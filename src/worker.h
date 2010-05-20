/*

Copyright (C) 2010 Francesco Balestrieri

This file is part of Flexo - a time tracking application.

Flexo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef WORKER_H
#define WORKER_H

#include <QTime>

#ifdef WORKER_TEST
extern QDateTime clock_;
#endif

class QDateTime;

/*
 * This class represents a worker. A worker checks in and checks out from work multiple times in a
 * working day, and needs to complete a predefined amount of hours on each day it is at work.
 * As the worker keeps working, it accrues a balance of hours which can be positive or negative based
 * on how much work was done compared to what was expected.
 *
 * A working day doesn't have a pre-defined start or end time, but any work done before 5am is considered
 * to belong to the previous day.
 *
 * If a worker doesn't check in at all during a day, that day is not considered in the balance calculation.
 *
 * If a worker checks in during the weekend, any work done is considered as overtime (i.e. increases the balance)
 *
 * The expected amount of work during a day can be changed.
 * The new value doesn't affect balance calculation for past workdays.
 *
 * For simplicity, if the worker checks out after more than 24 hours since the last checkin,
 * this is considered an error
 */
class Worker {

public:

    Worker();
    Worker(const Worker&);

    ~Worker();

    Worker& operator=(const Worker&);

    void clear();

    /*
     * start working. If already working, do nothing.
     *
     * If this is the first checkin of the day, the amount of work expected for the day
     * is subtracted from the current balance.
     *
     * Returns the checkin time
     */
    QDateTime checkin();

    /*
     * Stop working. If not working at the moment, do nothing.
     *
     * The balance is updated when the user checks out. If the last
     * checkin was in the previous day, the last checkin time is udpated to 5 am
     * of the current day and a new day starts.
     *
     * Returns the amount of work done since the last checkin, in seconds. If the time since the
     * last checkin is greater than 24 hours, returns -1
     */
    int checkout();

    /*
     * Returns the total amount of work done in the current workday, in seconds,
     * until the last checkout. If the last checkin was in the previous day, returns 0
     */
    int workDoneToday() const;

    /*
     * Returns the work in progress, i.e. work since the last checkin.
     * Returns 0 if the worker isn't working
     */
    int workInProgress() const;

    /*
     * Returns the total amount of work done vs. work planned so far until the last checkout,
     * in seconds.
     *
     * Returns a negative value if the worker has slacked
     */
    int balance() const;

    /*
     * Returns the balance in progress (in seconds), i.e. what the balance would be if the worker
     * checked out now.
     *
     * Returns a negative value if the worker is slacking
     */
    int balanceInProgress() const;

    /*
     * Returns true if the worker is currently doing overtime work
     */
    bool isOvertime() const;

    /*
     * Returns a pointer to the last checkin time, or null if the worker
     * never checked in
     */
    const QDateTime* lastCheckin() const;

    /*
     * Returns a pointer to the last checkout time, or null if the worker
     * never checked out
     */
    const QDateTime* lastCheckout() const;

    /*
     * Returns true if the current time is a holiday
     */
    bool isHoliday() const;

    /*
     * Returns true if the date given as parameter is a holiday
     */
    bool isHoliday(const QDateTime&) const;

    // assorted accessor methods generally used at initialization and when storing state
    // (probably would make sense to have state saving functions instead)
    void setLastCheckin(const QDateTime&);
    void setLastCheckout(const QDateTime&);
    int workdayLength() const;
    void setWorkdayLength(int);
    void setBalance(int);
    void setWorkDoneToday(int);
    bool isWorking() const;
    void setWorking(bool b) { working_ = b; };

private:

    bool working_;
    QDateTime *lastCheckin_;
    QDateTime *lastCheckout_;

    int workDoneToday_;

    int workdayLength_;
    int balance_;
    bool isHoliday_;

    static const QTime startTime_;

    void copy(const Worker&);

};
#endif // WORKER_H
