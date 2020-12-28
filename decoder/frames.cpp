#include "frames.h"
extern "C"
{
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
}

Frames::Frames()
{

}

Frames::~Frames()
{

}

bool Frames::init()
{
    m_decodingFrame = av_frame_alloc();
    if (!m_decodingFrame) {
        goto error;
    }

    m_renderingFrame = av_frame_alloc();
    if (!m_renderingFrame) {
        goto error;
    }
    m_renderingFrameConsumed = true;
    return true;
error:
    deinit();
    return false;
}

void Frames::deinit()
{
    if (m_decodingFrame) {
        av_frame_free(&m_decodingFrame);
        m_decodingFrame = Q_NULLPTR;
    }
    if (m_renderingFrame) {
        av_frame_free(&m_renderingFrame);
        m_renderingFrame = Q_NULLPTR;
    }
}

void Frames::lock()
{
    m_mutex.lock();
}

void Frames::unLock()
{
    m_mutex.unlock();
}

AVFrame *Frames::decodingFrame()
{
    return m_decodingFrame;
}

bool Frames::offerDecodedFrame()
{
    m_mutex.lock();
    swap();
    bool previousFrameConsumed = m_renderingFrameConsumed;
    m_renderingFrameConsumed = false;
    m_mutex.unlock();
    return previousFrameConsumed;
}

const AVFrame *Frames::consumeRenderedFrame()
{
    Q_ASSERT(!m_renderingFrameConsumed);
    m_renderingFrameConsumed = true;
    return m_renderingFrame;
}

void Frames::stop()
{

}

void Frames::swap()
{
    AVFrame *tmp = m_decodingFrame;
    m_decodingFrame = m_renderingFrame;
    m_renderingFrame = tmp;
}
