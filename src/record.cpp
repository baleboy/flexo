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
#include <assert.h>

#include "record.h"
#include "constants.h"

Record::Record()
{
    m_checkinTime = CURRENT_TIME;
}

Record::Record(const QDateTime &t)
    : m_checkinTime(t)
{

}

Record::Record(const Record &r)
{
    copy(r);
}

Record& Record::operator=(const Record& r)
{
    if (this != &r)
        copy(r);
    return *this;
}

int Record::elapsed() const
{
    assert(m_checkinTime.isValid());

    if (inProgress())
        return m_checkinTime.secsTo(CURRENT_TIME);

    return m_checkinTime.secsTo(m_checkoutTime);
}

bool Record::inProgress() const
{
    return (m_checkoutTime.isNull());
}

int Record::setCheckinTime(const QDateTime &t)
{
    if (!inProgress() && (t > m_checkoutTime))
        return -1;

    m_checkinTime = t;
    return 0;
}

int Record::setCheckoutTime(const QDateTime &t)
{
    if (t < m_checkinTime)
        return -1;

    m_checkoutTime = t;
    return 0;
}

QDateTime Record::checkinTime() const
{
    return m_checkinTime;
}

QDateTime Record::checkoutTime() const
{
    return m_checkoutTime;
}

void Record::copy(const Record &r)
{
    m_checkinTime = r.m_checkinTime;
    m_checkoutTime = r.m_checkoutTime;
}

QDataStream& operator<< (QDataStream& out, const Record& r)
{
    out << r.m_checkinTime << r.m_checkoutTime;
    return out;
}

QDataStream& operator>> (QDataStream& in, Record& r)
{
    in >> r.m_checkinTime >> r.m_checkoutTime;
    return in;
}
