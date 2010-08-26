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
#include <QDataStream>

#include "recordstack.h"
#include "record.h"
#include "constants.h"

RecordStack::RecordStack()
{
}

int RecordStack::push(const Record& r)
{
    if (m_stack.size() > 0) {
        if (m_stack.top().inProgress())
            return -1; // can't check-in if working

        if (m_stack.top().checkoutTime() > r.checkinTime())
            return -1;
    }
    m_stack.push(r);
    return 0;
}

Record RecordStack::pop()
{
    return m_stack.pop();
}

const Record& RecordStack:: top() const
{
    return m_stack.top();
}

const Record& RecordStack::at(int i) const
{
    return m_stack.at(i);
}

int RecordStack::size() const
{
    return m_stack.size();
}

bool RecordStack::isEmpty() const
{
    return m_stack.isEmpty();
}

int RecordStack::replace(int i, const Record &r)
{
    if ((i < 0) || (i >= m_stack.size()))
        return -1;

    if (m_stack.size() > 1) {
        if (i != 0) { // not the first record
            if (r.checkinTime() < m_stack.at(i-1).checkoutTime())
                return -1;
        }
        if (i != m_stack.size() - 1) { // not the topmost record
            if ((r.checkoutTime() > m_stack.at(i+1).checkinTime()) || r.inProgress())
                return -1;
        }
    }

    m_stack.replace(i, r);
    return 0;
}

void RecordStack::clear()
{
    m_stack.clear();
}

QDataStream& operator<< (QDataStream& out, const RecordStack& r)
{
    out << r.m_stack;
    return out;
}

QDataStream& operator>> (QDataStream& in, RecordStack& r)
{
    in >> r.m_stack;
    return in;
}
