#include <QBuffer>
#include "controlevent.h"
#define TEXT_MAX_CHARCTER_LENGTH    (1024)

ControlEvent::ControlEvent(ControlEventType type)
    : QScrcpyEvent(Control)
{
    m_data.type = type;
}

void ControlEvent::setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode, AndroidMetastate metastate)
{
    m_data.keycodeEvent.action = action;
    m_data.keycodeEvent.keycode = keycode;
    m_data.keycodeEvent.metastate = metastate;
}

void ControlEvent::setMouseEventData(AndroidMotioneventAction action, AndroidMotioneventButtons buttons, QRect position)
{
    m_data.mouseEvent.action = action;
    m_data.mouseEvent.buttons = buttons;
    m_data.mouseEvent.position = position;
}

void ControlEvent::setTextEventData(QString text)
{
    m_data.textEvent.text = text;
}

void ControlEvent::setTouchEventData(quint32 id, AndroidMotioneventAction action, QRect position)
{
    m_data.touchEvent.id = id;
    m_data.touchEvent.action = action;
    m_data.touchEvent.position = position;
}

void ControlEvent::setScrollEventData(QRect position, qint32 hScroll, qint32 vScroll)
{
    m_data.scrollEvent.position = position;
    m_data.scrollEvent.hScroll = hScroll;
    m_data.scrollEvent.vScroll = vScroll;
}

void ControlEvent::setCommandEventData(qint32 action)
{
    m_data.commandEvent.action = action;
}

QByteArray ControlEvent::serializeData()
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::WriteOnly);

    buffer.putChar(m_data.type);
    switch (m_data.type) {
    case ControlEventMouse:
        buffer.putChar(m_data.mouseEvent.action);
        write32(buffer, m_data.mouseEvent.buttons);
        writePosition(buffer, m_data.mouseEvent.position);
        break;
    case ControlEventKeyCode:
        buffer.putChar(m_data.keycodeEvent.action);
        write16(buffer, m_data.keycodeEvent.keycode);
        write16(buffer, m_data.keycodeEvent.metastate);
        break;
    case ControlEventScroll:
        break;
    case ControlEventText:
    {
        if (TEXT_MAX_CHARCTER_LENGTH < m_data.textEvent.text.length()) {
            m_data.textEvent.text = m_data.textEvent.text.left(TEXT_MAX_CHARCTER_LENGTH);
        }
        QByteArray tmp = m_data.textEvent.text.toUtf8();
        write16(buffer, tmp.length());
        buffer.write(tmp.data(), tmp.length());
    }
        break;
    case ControlEventTouch:
        buffer.putChar(m_data.touchEvent.id);
        buffer.putChar(m_data.touchEvent.action);
        writePosition(buffer, m_data.touchEvent.position);
        break;
    case ControlEventCommand:
        buffer.putChar(m_data.commandEvent.action);
        break;
    default:
        break;
    }
    buffer.close();
    return byteArray;
}

void ControlEvent::write32(QBuffer &buffer, quint32 data)
{
    buffer.putChar(data >> 24);
    buffer.putChar(data >> 16);
    buffer.putChar(data >> 8);
    buffer.putChar(data >> 0);
}

void ControlEvent::write16(QBuffer &buffer, quint16 data)
{
    buffer.putChar(data >> 8);
    buffer.putChar(data >> 0);
}

void ControlEvent::writePosition(QBuffer &buffer, QRect &position)
{
    write16(buffer, position.x());
    write16(buffer, position.y());
    write16(buffer, position.width());
    write16(buffer, position.height());
}
