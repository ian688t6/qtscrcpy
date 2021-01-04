#ifndef CONTROLEVENT_H
#define CONTROLEVENT_H
#include <QRect>
#include <QByteArray>
#include <QBuffer>
#include "input.h"
#include "keycodes.h"
#include "qscrcpyevent.h"

class ControlEvent : public QScrcpyEvent
{
public:
    enum ControlEventType {
        ControlEventKeyCode = 0,
        ControlEventText,
        ControlEventMouse,
        ControlEventScroll,
        ControlEventCommand,
        ControlEventTouch,
    };
    ControlEvent(ControlEventType type);
    void setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode, AndroidMetastate metastate);
    void setMouseEventData(AndroidMotioneventAction action, AndroidMotioneventButtons buttons, QRect position);
    void setTextEventData(QString text);
    void setTouchEventData(quint32 id, AndroidMotioneventAction action, QRect position);
    void setScrollEventData(QRect position, qint32 hScroll, qint32 vScroll);
    void setCommandEventData(qint32 action);
    QByteArray serializeData();

protected:
    void write32(QBuffer &buffer, quint32 data);
    void write16(QBuffer &buffer, quint16 data);
    void writePosition(QBuffer &buffer, QRect &position);

private:
    struct ControlEventData {
        ControlEventType type;
        union {
            struct
            {
                AndroidKeyeventAction action;
                AndroidKeycode keycode;
                AndroidMetastate metastate;
            } keycodeEvent;
            struct {
                QString text;
            } textEvent;
            struct {
                AndroidMotioneventAction action;
                AndroidMotioneventButtons buttons;
                QRect position;
            } mouseEvent;
            struct {
                quint32 id;
                AndroidMotioneventAction action;
                QRect position;
            } touchEvent;
            struct {
                QRect position;
                qint32 hScroll;
                qint32 vScroll;
            } scrollEvent;
            struct {
                qint32 action;
            } commandEvent;
        };
        ControlEventData() {}
        ~ControlEventData() {}
    };
    ControlEventData m_data;
};

#endif // CONTROLEVENT_H
