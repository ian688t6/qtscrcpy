#ifndef FRAMES_H
#define FRAMES_H
#include <QMutex>
#include <QWaitCondition>
typedef struct AVFrame AVFrame;

class Frames
{
public:
    Frames();
    virtual ~Frames();

    bool init();
    void deinit();
    void lock();
    void unLock();

    AVFrame *decodingFrame();
    bool offerDecodedFrame();
    const AVFrame *consumeRenderedFrame();
    void stop();

private:
    void swap();

private:
    AVFrame *m_decodingFrame = Q_NULLPTR;
    AVFrame *m_renderingFrame = Q_NULLPTR;
    QMutex m_mutex;
    bool m_renderingFrameConsumed = true;
};

#endif // FRAMES_H
