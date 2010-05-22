/*

Copyright (C) 2010 Francesco Balestrieri

This file is part of Flexo - a time tracking application.

Flexo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Flexo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Flexo.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef ALARM_H
#define ALARM_H

#include <QString>
#include <QDateTime>

class QDateTime;

/*!
 * Generic interface for alarms. At a minimum, an alarm has a time and a message,
 * and can be set or removed.
 */
class Alarm
{
public:

    Alarm() {};

    void setMessage(const QString& m) { m_message = m; }
    void setTime(const QDateTime& t) {m_time = t; }

    QString message() const { return m_message; }
    QDateTime time() const { return m_time; }

    virtual void set() = 0;
    virtual void remove() = 0;

    virtual bool isSet() const = 0;

private:

    QString m_message;
    QDateTime m_time;

};

#endif // ALARM_H
