#ifndef QYUVOPENGLWIDGET_H
#define QYUVOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class QYUVOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QYUVOpenGLWidget(QWidget *parent = nullptr);
    virtual ~QYUVOpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setFrameSize(const QSize& frameSize);
    const QSize& frameSize();
    void updateTextures(quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 lineSizeY, quint32 lineSizeU, quint32 lineSizeV);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    void initShader();
    void initTextures();
    void deInitTextures();
    void updateTexture(GLuint texture, quint32 textureType, quint8 *pixels, quint32 stride);

private:
    QSize m_frameSize = {-1, -1};
    bool m_needUpdate = false;
    bool m_textureInited = false;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram m_shaderProgram;
    GLuint m_textures[3] = {0};
};

#endif // QYUVOPENGLWIDGET_H
