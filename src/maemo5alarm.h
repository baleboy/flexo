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

#ifndef MAEMO5ALARM_H
#define MAEMO5ALARM_H

#include "alarm.h"

struct alarm_event_t;

typedef long alarm_id_t;

/*!
 * Implementation of the alarm interface for the Maemo 5 platform. Provides a wrapper for
 * libalarm.h
 */
class Maemo5Alarm : public Alarm
{
public:
    Maemo5Alarm(const QString&);
    Maemo5Alarm(const Alarm&);
    Maemo5Alarm(alarm_id_t);

    ~Maemo5Alarm();
    Maemo5Alarm& operator=(const Maemo5Alarm&);

    void setSnoozeTime(int);

    void addDismissAction(const QString&);
    void addSnoozeAction(const QString&);
    void addExecAction(const QString& text, const QString& command);
    void addDBusAction(const QString& text, const QString& service,
                       const QString& interface, const QString& path,
                       const QString& name);

    void set();
    void remove();

    bool isSet() const {return m_set; };

    static void getAlarms(const QString&, QList<alarm_id_t>&);

private:

    alarm_event_t *m_data;
    bool m_set;

    void copy(const Maemo5Alarm&);
};

#endif // Maemo5Alarm_H
