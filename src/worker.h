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

#ifndef WORKER_H
#define WORKER_H

#include <QTime>

#include "recordstack.h"

class QDateTime;

/*!
 * This class represents a worker. A worker checks in and checks out from work multiple times in a
 * working day, and needs to complete a predefined amount of hours on each day it is at work.
 * As the worker keeps working, it accrues a balance of hours which can be positive or negative based
 * on how much work was done compared to what was expected.
 *
 * If a worker doesn't check in at all during a day, that day is not considered in the balance calculation.
 *
 * If a worker checks in during the weekend, any work done is considered as overtime (i.e. increases the balance)
 *
 * The expected amount of work during a day can be changed, in which case the balance is
 * calculated according to the new value
 *
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
     * Returns the checkin time
     */
    QDateTime checkin();

    /*
     * Stop working. If not working at the moment, do nothing.
     *
     * Returns the amount of work done since the last checkin, in seconds. Returns
     * -1 if there has been no check in at all.
     *
     */
    int checkout();

    /*
     * Returns the total amount of work done in the current workday, in seconds,
     * until the last checkout. If the current checkin was in the previous day,
     * returns 0.
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

    /**
     * Set the current balance
     */
    void setBalance(int);

    /*
     * Returns true if the worker is currently doing overtime work
     */
    bool isOvertime() const;

    /*
     * Returns the current checkin time. The returned datetime object is not valid if the worker
     * never checked in, or if the worker is currently checked out.
     */
    QDateTime currentCheckin() const;

    /*
     * Returns the current checkout time. The returned datetime object is not valid if the worker
     * never checked out, or if the worker is currently checked in.
     */
    QDateTime currentCheckout() const;

    /*
     * Returns true if the current time is a holiday
     */
    bool isHoliday() const;

    /*
     * Returns true if the date given as parameter is a holiday
     */
    bool isHoliday(const QDate&) const;

    QString print() const;

    /*!
     * Update time of the current checkin.
     *
     * Returns -1 if the worker is not working, or if the new checkin time
     * is earlier than the last checkout, 0 otherwise.
     */
    int updateCheckinTime(const QDateTime&);

    /*!
     * Update time of the current checkout.
     *
     * Returns -1 if the checkout time is less than the checkin time,
     * or if the worker is still working, 0 otherwise.
     */
    int updateCheckoutTime(const QDateTime&);

    int workdayLength() const;
    void setWorkdayLength(int);
    bool isWorking() const;

    friend QDataStream& operator<< (QDataStream&, const Worker&);
    friend QDataStream& operator>> (QDataStream&, Worker&);

private:

    RecordStack m_recordStack;

    quint32 m_workdayLength;
    quint32 m_balanceAdjustment;

    void copy(const Worker&);

    int balanceFromHistory() const;
};

QDataStream& operator<< (QDataStream&, const Worker&);

QDataStream& operator>> (QDataStream&, Worker&);

#endif // WORKER_H
