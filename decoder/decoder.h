#ifndef DECODER_H
#define DECODER_H
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class Decoder
{
public:
    Decoder();

    static bool init();
};

#endif // DECODER_H
