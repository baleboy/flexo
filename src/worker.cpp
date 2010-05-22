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

#include "worker.h"

// if compiled for test, replace the actual clock with a global variable
// that can be manipulated by the test program
#ifdef WORKER_TEST
QDateTime clock_;
#define CURRENT_TIME clock_
#else
#define CURRENT_TIME QDateTime::currentDateTime()
#endif

#define DAY_IN_SECS (24*3600)
#define DEFAULT_WORKDAY (3600*7.5)

const QTime Worker::startTime_(QTime::fromString("05:00:00", "hh:mm:ss"));

Worker::Worker()
    : working_(0), lastCheckin_(0), lastCheckout_(0), workDoneToday_(0),
      workdayLength_(DEFAULT_WORKDAY), balance_(0), isHoliday_(false)
{
}

Worker::Worker(const Worker &w)
{
    copy(w);
}

Worker::~ Worker()
{
    if (lastCheckin_) delete lastCheckin_;
    if (lastCheckout_) delete lastCheckout_;
}

Worker& Worker::operator=(const Worker& w)
{
    if (this != &w) {
        if (lastCheckin_) delete lastCheckin_;
        if (lastCheckout_) delete lastCheckout_;
        copy(w);
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
QDateTime Worker::checkin()
{
    bool firstCheckin = false;
    if (working_) {
        qDebug("Warning: checked in while already working");
    }
    else {
        if (!lastCheckin_) {
            lastCheckin_ = new QDateTime(CURRENT_TIME);
            firstCheckin = true;
        }
        QDateTime now = CURRENT_TIME;
        QDateTime dayStart = now;
        dayStart.setTime(startTime_);

        if (*lastCheckin_ < dayStart || firstCheckin ) {
            // first checkin of the day
            workDoneToday_ = 0;
            if (!isHoliday(now)) {
                balance_ -= workdayLength_;
            }
        }

        *lastCheckin_ = now;
        working_ = true;
    }
    return *lastCheckin_;
}

///////////////////////////////////////////////////////////////////////////////
int Worker::checkout()
{
    if (!working_) {
        qDebug("Warning: checked out while not working");
        return 0;
    }

    QDateTime t = CURRENT_TIME;
    int workSinceLastCheckin = lastCheckin_->secsTo(t);

    if ( workSinceLastCheckin > DAY_IN_SECS) {
        qDebug("Warning: checked out after more than 24 hrs");
        return -1;
    }

    QDateTime dayStart = t;
    dayStart.setTime(startTime_);

    if ((*lastCheckin_ < dayStart) && (t >= dayStart)) {
        if (!isHoliday(*lastCheckin_)) {
            balance_ -= workdayLength_;
        }
        workDoneToday_ = dayStart.secsTo(CURRENT_TIME);
        *lastCheckin_ = dayStart;
    }
    else {
        workDoneToday_ += workSinceLastCheckin;
    }
    balance_ += workSinceLastCheckin;
    working_ = false;

    if (!lastCheckout_) {
        lastCheckout_ = new QDateTime;
    }
    *lastCheckout_ = t;

    return workSinceLastCheckin;

}

///////////////////////////////////////////////////////////////////////////////
int Worker::balance() const
{
    return balance_;
}

///////////////////////////////////////////////////////////////////////////////
int Worker::workDoneToday() const
{
    if (lastCheckin_) {
        QDateTime t = CURRENT_TIME;
        QDateTime dayStart = t;
        dayStart.setTime(startTime_);
        if ((*lastCheckin_ < dayStart) && (t >= dayStart) && !working_) {
            return 0;
        }
    }
    return workDoneToday_;
}

///////////////////////////////////////////////////////////////////////////////
bool Worker::isWorking() const
{
    return working_;
}

///////////////////////////////////////////////////////////////////////////////
const QDateTime* Worker::lastCheckin() const
{
    return lastCheckin_;
}

///////////////////////////////////////////////////////////////////////////////
const QDateTime* Worker::lastCheckout() const
{
    return lastCheckout_;
}

///////////////////////////////////////////////////////////////////////////////
void Worker::setLastCheckin(const QDateTime& d)
{
    if (!lastCheckin_) {
        lastCheckin_ = new QDateTime(d);
    }
    else {
        *lastCheckin_ = d;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Worker::setLastCheckout(const QDateTime& d)
{
    if (!lastCheckout_) {
        lastCheckout_ = new QDateTime(d);
    }
    else {
        *lastCheckout_ = d;
    }
}

///////////////////////////////////////////////////////////////////////////////
int Worker::workdayLength() const
{
    return workdayLength_;
}

///////////////////////////////////////////////////////////////////////////////
void Worker::setWorkdayLength(int s)
{
    workdayLength_ = s;
}

///////////////////////////////////////////////////////////////////////////////
void Worker::setWorkDoneToday(int s)
{
    workDoneToday_ = s;
}

///////////////////////////////////////////////////////////////////////////////
int Worker::workInProgress() const
{
    return (working_ ? lastCheckin_->secsTo(CURRENT_TIME) : 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Worker::isOvertime() const
{
    int legalWork = isHoliday(CURRENT_TIME)? 0 : workdayLength_;
    return ((workDoneToday() + workInProgress() > legalWork) ||
            ((workDoneToday() + workInProgress() == legalWork) && isWorking()));
}

///////////////////////////////////////////////////////////////////////////////
bool Worker::isHoliday(const QDateTime& t) const
{
    return (t.date().dayOfWeek() == 6) || (t.date().dayOfWeek() == 7) || isHoliday_;
}

///////////////////////////////////////////////////////////////////////////////
bool Worker::isHoliday() const
{
    return (isHoliday(CURRENT_TIME));
}

///////////////////////////////////////////////////////////////////////////////
void Worker::setBalance(int s)
{
    balance_ = s;
}

///////////////////////////////////////////////////////////////////////////////
int Worker::balanceInProgress() const
{
    if (!lastCheckin_) {
       return 0;
    }

    if (!working_) {
        return balance_;
    }

    int currentBalance = balance_;
    QDateTime t = CURRENT_TIME;
    int workSinceLastCheckin = lastCheckin_->secsTo(t);

    if ( workSinceLastCheckin > DAY_IN_SECS) {
        return -1;
    }

    QDateTime dayStart = t;
    dayStart.setTime(startTime_);

    if ((*lastCheckin_ < dayStart) && (t >= dayStart) && !isHoliday(*lastCheckin_)) {
        currentBalance -= workdayLength_;
    }

    currentBalance += workSinceLastCheckin;

    return currentBalance;
}

///////////////////////////////////////////////////////////////////////////////
void Worker::copy(const Worker &w)
{
    working_ = w.working_;
    workDoneToday_ = w.workDoneToday_;
    workdayLength_ = w.workdayLength_;
    balance_ = w.balance_;
    isHoliday_ = w.isHoliday_;

    if (w.lastCheckin_) {
        lastCheckin_ = new QDateTime(*w.lastCheckin_);
    }
    else {
        lastCheckin_ = 0;
    }

    if (w.lastCheckout_) {
        lastCheckout_ = new QDateTime(*w.lastCheckout_);
    }
    else {
        lastCheckout_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Worker::clear()
{
    working_ = 0;
    if (lastCheckin_) {
        delete lastCheckin_;
        lastCheckin_ = 0;
    }
    if (lastCheckout_) {
        delete lastCheckout_;
        lastCheckout_ = 0;
    }

    workDoneToday_ = 0;
    workdayLength_ = DEFAULT_WORKDAY;
    balance_ = 0;
    isHoliday_ = false;
}

