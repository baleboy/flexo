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
along with Flexo. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef RECORD_H
#define RECORD_H

#include <QDateTime>

/**
 * Data related to one individual work period (check-in to check-out)
 */
class Record {

public:

    Record();
    Record(const QDateTime&);
    Record(const Record&);

    Record& operator=(const Record&);

    /**
     * Returns the seconds elapsed between check-in and check-out. If
     * the record is still active (the worker hasn't checked out) returns
     * the work in progress, i.e. seconds between check-in and current time
     */
    int elapsed() const;

    /**
     * Returns true if the record doesn't have a check-out time yet
     */
    bool inProgress() const;

    /**
     * Update check-in time
     *
     * Returns 0 if the update was successful, -1 if the record
     * is not active and the check-in time is later than the check-out time
     */
    int setCheckinTime(const QDateTime&);

    /**
     * Update check-out time
     *
     * Returns 0 if the update was successful, -1 if the check-out time
     * is earlier than the check-in time
     */
    int setCheckoutTime(const QDateTime&);

    QDateTime checkinTime() const;
    QDateTime checkoutTime() const;

    friend QDataStream& operator<< (QDataStream&, const Record&);
    friend QDataStream& operator>> (QDataStream&, Record&);

private:

    void copy(const Record&);

    QDateTime m_checkinTime;
    QDateTime m_checkoutTime;
};

QDataStream& operator<< (QDataStream&, const Record&);
QDataStream& operator>> (QDataStream&, Record&);

#endif // RECORD_H
