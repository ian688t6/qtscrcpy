#include "decoder.h"
#include "devicesocket.h"
#include "frames.h"
#define BUFSIZE (0x10000)

Decoder::Decoder()
{

}

Decoder::~Decoder()
{

}

bool Decoder::init()
{
    if (avformat_network_init()) {
        return false;
    }
    return true;
}

bool Decoder::deinit()
{
    avformat_network_deinit();
    return true;
}

void Decoder::setFrame(Frames *frames)
{
    m_frames = frames;
}

static qint32 readPacket(void *opaque, quint8 *buf, qint32 bufSize) {
    Decoder *decoder = (Decoder *)opaque;
    if (decoder) {
        return decoder->recvData(buf, bufSize);
    }
    return 0;
}

void Decoder::setDeviceSocket(DeviceSocket *socket)
{
    m_deviceSocket = socket;
}

qint32 Decoder::recvData(quint8 *buf, qint32 bufSize)
{
    if (!buf) {
        return 0;
    }
    if (m_deviceSocket) {
        qint32 len = m_deviceSocket->subThreadRecvData(buf, bufSize);
        if (len == -1) {
            return AVERROR(errno);
        }
        if (len == 0) {
            return AVERROR_EOF;
        }
        return len;
    }
    return AVERROR_EOF;
}

bool Decoder::startDecoder()
{
    if (!m_deviceSocket) {
        return false;
    }
    m_quit = false;
    start();
    return true;
}

void Decoder::stopDecoder()
{
    m_quit = true;
    if (m_frames) {
        m_frames->stop();
    }
    wait();
}

void Decoder::run()
{
    unsigned char *decoderBuffer = Q_NULLPTR;
    AVIOContext *avioCtx = Q_NULLPTR;
    AVFormatContext *formatCtx = Q_NULLPTR;
    AVCodec *codec = Q_NULLPTR;
    AVCodecContext *codecCtx = Q_NULLPTR;
    bool isFormatCtxOpen = false;
    bool isCodecCtxOpen = false;

    decoderBuffer = (unsigned char *)av_malloc(BUFSIZE);
    if (!decoderBuffer) {
        qCritical("Could not allocate buffer");
        goto runQuit;
    }

    avioCtx = avio_alloc_context(decoderBuffer, BUFSIZE, 0, this, readPacket, NULL, NULL);
    if (!avioCtx) {
        qCritical("Could not allocate avio context");
        av_free(decoderBuffer);
        goto runQuit;
    }

    formatCtx = avformat_alloc_context();
    if (!formatCtx) {
        qCritical("Could not allocate formate context");
        goto runQuit;
    }
    formatCtx->pb = avioCtx;

    if (avformat_open_input(&formatCtx, NULL, NULL, NULL)) {
        qCritical("Could not open video stream");
        goto runQuit;
    }
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qCritical("H.264 decoder not found");
        goto runQuit;
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        qCritical("could not alloc codec Ctx");
        goto runQuit;
    }

    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        qCritical("Could not open H.264 codec");
        goto runQuit;
    }
    isCodecCtxOpen = true;
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = Q_NULLPTR;
    packet.size = 0;
    while (!m_quit && !av_read_frame(formatCtx, &packet)) {
        AVFrame *decodingFrame = m_frames->decodingFrame();
        int ret;
        if ((ret = avcodec_send_packet(codecCtx, &packet)) < 0) {
            qCritical("Could not send video packet: %d", ret);
            goto runQuit;
        }
        if (decodingFrame) {
            ret = avcodec_receive_frame(codecCtx, decodingFrame);
        }
        if (!ret) {
            pushFrame();
        } else if (ret != AVERROR(EAGAIN)){
            qCritical("Could not receive video frame: %d", ret);
            av_packet_unref(&packet);
            goto runQuit;
        }

        av_packet_unref(&packet);
        if (avioCtx->eof_reached) {
            break;
        }
    }
    qDebug() << "End of frames";
runQuit:
    if (avioCtx) {
        av_freep(&avioCtx);
    }

    if (formatCtx && isFormatCtxOpen) {
        avformat_close_input(&formatCtx);
    }

    if (formatCtx) {
        avformat_free_context(formatCtx);
    }

    if (codecCtx && isCodecCtxOpen) {
        avcodec_close(codecCtx);
    }

    if (codecCtx) {
        avcodec_free_context(&codecCtx);
    }
    emit onDecodeStop();
}

void Decoder::pushFrame()
{
    bool previousFrameConsumed = m_frames->offerDecodedFrame();
    if (!previousFrameConsumed) {
        return;
    }
    emit onNewFrame();
}
