#ifndef ALARM_H
#define ALARM_H

#include <QString>
#include <QDateTime>

class QDateTime;

class Alarm
{
public:

    Alarm() {};

    void setMessage(const QString& m) { m_message = m; }
    void setTime(const QDateTime& t) {m_time = t; }

    QString message() { return m_message; }
    QDateTime time() { return m_time; }

    virtual void set() = 0;
    virtual void remove() = 0;

private:

    QString m_message;
    QDateTime m_time;

};

#endif // ALARM_H
