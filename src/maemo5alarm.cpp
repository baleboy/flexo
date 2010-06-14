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

#include <QString>
#include <QDateTime>
#include <QtDebug>
#include <QList>

#include <alarmd/libalarm.h>

#include "maemo5alarm.h"

Maemo5Alarm::Maemo5Alarm(const QString& myapp)
    : m_set(false)
{
    m_data = alarm_event_create();
    alarm_event_set_alarm_appid(m_data, myapp.toAscii().constData());
}

Maemo5Alarm::Maemo5Alarm(const Alarm &other)
{
    m_data = alarm_event_create();
    copy(other);
}

Maemo5Alarm::Maemo5Alarm(alarm_id_t cookie)
{
    if (cookie != 0) {
        m_data = alarmd_event_get(cookie);
        if (m_data) m_set = true;
    }
}

Maemo5Alarm::~Maemo5Alarm()
{
    alarm_event_delete(m_data);
}

Maemo5Alarm& Maemo5Alarm::operator=(const Maemo5Alarm &other)
{
    if(this != &other) {
        copy(other);
    }
    return *this;
}

void Maemo5Alarm::copy(const Maemo5Alarm& other)
{
    alarm_event_set_alarm_appid(m_data, other.m_data->alarm_appid);
    alarm_event_set_message(m_data, other.m_data->message);
    m_data->alarm_time = other.m_data->alarm_time;

    alarm_action_t *act, *other_act;
    if (other.m_data->action_cnt > 0) {
         for (quint32 i = 0 ; i < other.m_data->action_cnt ; ++i) {
             act = alarm_event_add_actions(m_data, 1);
             other_act = alarm_event_get_action(other.m_data, i);
             act->flags = other_act->flags;
             alarm_action_set_label(act, other_act->label);
             alarm_action_set_exec_command(act, other_act->exec_command);
         }
    }
    m_set = other.m_set;
}

void Maemo5Alarm::setSnoozeTime(int secs)
{
    m_data->snooze_secs = secs;
}

void Maemo5Alarm::addDismissAction(const QString& text)
{
    alarm_action_t *action;

    action = alarm_event_add_actions(m_data, 1);
    alarm_action_set_label(action, text.toAscii().constData());
    action->flags = ALARM_ACTION_WHEN_RESPONDED | ALARM_ACTION_TYPE_NOP;
}

void Maemo5Alarm::addSnoozeAction(const QString& text)
{
    alarm_action_t *action;

    action = alarm_event_add_actions(m_data, 1);
    alarm_action_set_label(action, text.toAscii().constData());
    action->flags = ALARM_ACTION_WHEN_RESPONDED | ALARM_ACTION_TYPE_SNOOZE;
}

void Maemo5Alarm::addExecAction(const QString& text, const QString& command)
{
    alarm_action_t *action;

    action = alarm_event_add_actions(m_data, 1);
    alarm_action_set_label(action, text.toAscii().constData());
    action->flags = ALARM_ACTION_WHEN_RESPONDED | ALARM_ACTION_TYPE_EXEC;
    alarm_action_set_exec_command(action, command.toAscii().constData());
}

void Maemo5Alarm::addDBusAction(const QString& text, const QString& service,
                                const QString& interface, const QString& path,
                                const QString& name)
{
    alarm_action_t *action;

    action = alarm_event_add_actions(m_data, 1);
    alarm_action_set_label(action, text.toAscii().constData());
    action->flags = ALARM_ACTION_WHEN_RESPONDED |
                    ALARM_ACTION_DBUS_USE_ACTIVATION | ALARM_ACTION_TYPE_DBUS;
    alarm_action_set_dbus_interface(action, interface.toAscii().constData());
    alarm_action_set_dbus_service(action, service.toAscii().constData());
    alarm_action_set_dbus_path(action, path.toAscii().constData());
    alarm_action_set_dbus_name(action, name.toAscii().constData());
}

void Maemo5Alarm::set()
{
    m_data->alarm_time = time().toTime_t();
    m_data->cookie = alarmd_event_update(m_data);
    if (!m_data->cookie) {
        qDebug() << "Error adding alarm: " << m_data->cookie;
        return;
    }
    m_set = true;
    qDebug() << "Alarm set for " << time().toString()
             << "with cookie " << m_data->cookie;
}

void Maemo5Alarm::remove()
{
    if (m_set) {
        if( alarmd_event_del(m_data->cookie) == -1 )
        {
            qDebug() << "Unable to delete cookie" << m_data->cookie;
            return;
        }
        m_set = false;
    }
}

void Maemo5Alarm::getAlarms(const QString& appid, QList<alarm_id_t>& list)
{
    cookie_t *cookie_list = 0;
    cookie_t cookie = 0;

    cookie_list = alarmd_event_query(0,0, 0,0, appid.toAscii().constData());

    for( int i = 0; (cookie = cookie_list[i]) != 0; ++i ) {
        list.append(cookie);
    }
    free(cookie_list);

    qDebug() << "Found " << list.count() << " alarms";
}

