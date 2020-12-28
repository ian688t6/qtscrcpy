#include "decoder.h"

Decoder::Decoder()
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
}

void Decoder::setFrame(Frames *frames)
{
    m_frames = frames;
}
