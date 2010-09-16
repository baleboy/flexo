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

#include <QDateTime>
#include <QtDebug>
#include <assert.h>

#include "worker.h"
#include "record.h"
#include "constants.h"

#define DAY_IN_SECS (24*3600)

Worker::Worker()
{
    clear();
}

Worker::Worker(const Worker &w)
{
    copy(w);
}


Worker::~ Worker()
{

}

Worker& Worker::operator=(const Worker& w)
{
    if (this != &w) {
        copy(w);
    }
    return *this;
}

QDateTime Worker::checkin()
{
    if (!m_recordStack.isEmpty()) {
        if (m_recordStack.top().inProgress())
            return m_recordStack.top().checkinTime();
    }
    Record r(CURRENT_TIME);
    int res = m_recordStack.push(r);
    assert(res == 0);
    return m_recordStack.top().checkinTime();
}

int Worker::checkout()
{
    if (m_recordStack.isEmpty())
        return -1;

    if (m_recordStack.top().inProgress()) {
        Record r = m_recordStack.top();
        int res = r.setCheckoutTime(CURRENT_TIME);
        assert(res == 0);
        res = m_recordStack.replace(m_recordStack.size() - 1, r);
        assert(res == 0);
    }
    return m_recordStack.top().elapsed();
}

int Worker::balanceFromHistory() const
{
    if (m_recordStack.isEmpty() ||
        (m_recordStack.size() == 1 && m_recordStack.top().inProgress()))
        return 0;

    QDate lastDay;
    int lastRecord;
    if (m_recordStack.top().inProgress()) {
        lastDay = m_recordStack.at(m_recordStack.size() - 2).checkoutTime().date();
        lastRecord = m_recordStack.size() - 2;
    }
    else {
        lastDay = m_recordStack.top().checkoutTime().date();
        lastRecord = m_recordStack.size() - 1;
    }

    int balance = 0;
    QDate currentDay;
    for (int i = 0; i <= lastRecord; ++i) {
        // at every day change that is a working day, subtract the balance
        if (m_recordStack.at(i).checkinTime().date() != currentDay) {
            currentDay = m_recordStack.at(i).checkinTime().date();
            if (!isHoliday(currentDay))
                balance -= m_workdayLength;
        }
        balance += m_recordStack.at(i).elapsed();
        qDebug() << "elapsed: " << m_recordStack.at(i).elapsed();
    }

    return balance;
}

int Worker::balance() const
{
    return m_balanceAdjustment + balanceFromHistory();
}

void Worker::setBalance(int b)
{
    m_balanceAdjustment = b - balanceFromHistory();
}

int Worker::workDoneToday() const
{   
    int done = 0;
    int i = m_recordStack.size() - 1;
    while (i >= 0) {
        if (m_recordStack.at(i).checkinTime().date() != CURRENT_TIME.date())
            break;
        if (!m_recordStack.at(i).inProgress())
            done += m_recordStack.at(i).elapsed();
        --i;
    }
    return done;
}

bool Worker::isWorking() const
{
    if (m_recordStack.isEmpty()) return false;

    return (m_recordStack.top().inProgress());
}

QDateTime Worker::currentCheckin() const
{
    QDateTime res;

    if (!m_recordStack.isEmpty())
        res = m_recordStack.top().checkinTime();

    return res;
}

QDateTime Worker::currentCheckout() const
{
    QDateTime res;

    if (!m_recordStack.isEmpty())
        res = m_recordStack.top().checkoutTime();

    return res;
}

int Worker::updateCheckinTime(const QDateTime& t)
{
    Record r = m_recordStack.top();
    int res = r.setCheckinTime(t);
    if (res != -1)
        res = m_recordStack.replace(m_recordStack.size() - 1, r);
    return res;
}

int Worker::updateCheckoutTime(const QDateTime& t)
{
    Record r = m_recordStack.top();
    int res = r.setCheckoutTime(t);
    if (res != -1)
        res = m_recordStack.replace(m_recordStack.size() - 1, r);
    return res;
}

int Worker::workdayLength() const
{
    return m_workdayLength;
}

void Worker::setWorkdayLength(int s)
{
    m_workdayLength = s;
}

int Worker::workInProgress() const
{
    return (isWorking() ? m_recordStack.top().elapsed() : 0);
}

bool Worker::isOvertime() const
{
    int legalWork = isHoliday() ? 0 : m_workdayLength;

    int total = workDoneToday() + workInProgress();
    return ((total > legalWork) ||
            ((total == legalWork) && isWorking()));
}

bool Worker::isHoliday(const QDate& d) const
{
    return (d.dayOfWeek() == 6) || (d.dayOfWeek() == 7);
}

bool Worker::isHoliday() const
{
    return (isHoliday(CURRENT_TIME.date()));
}

void Worker::copy(const Worker &w)
{
    m_recordStack = w.m_recordStack;
    m_workdayLength = w.m_workdayLength;
    m_balanceAdjustment = w.m_balanceAdjustment;
}

void Worker::clear()
{
    m_recordStack.clear();
    m_workdayLength = DEFAULT_WORKDAY;
    m_balanceAdjustment = 0;
}

QString Worker::print() const
{
    QString s;
    QTextStream out(&s);
    out << "workday length: " << m_workdayLength << endl;
    out << "balance adjustment: " << m_balanceAdjustment << endl;

    for (int i = 0 ; i < m_recordStack.size() ; ++i) {
        out << "Record [" << i << "]: in "
            << m_recordStack.at(i).checkinTime().toString()
            << ", out " << m_recordStack.at(i).checkoutTime().toString()
            << endl;

    }
    return s;
}

int Worker::records() const
{
    return m_recordStack.size();
}

QDateTime Worker::checkinAt(int i) const
{
    if (i < 0 || i >= records())
        return QDateTime();

    return m_recordStack.at(i).checkinTime();
}

QDateTime Worker::checkoutAt(int i) const
{
    if (i < 0 || i >= records())
        return QDateTime();

    return m_recordStack.at(i).checkoutTime();
}

QDataStream& operator>> (QDataStream& in, Worker& w)
{
    in >> w.m_recordStack >> w.m_workdayLength >> w.m_balanceAdjustment;
    return in;
}

QDataStream& operator<< (QDataStream& out, const Worker& w)
{
    out << w.m_recordStack
        << quint32(w.m_workdayLength)
        << quint32(w.m_balanceAdjustment);
    return out;
}
