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

#ifndef RECORDSTACK_H
#define RECORDSTACK_H

#include <QStack>

/**
 * A stack with some additional checks to ensure that check-in and check-out
 * times don't overlap
 */
class QDateTime;
class Record;

class RecordStack
{
public:
    RecordStack();

    /**
     * Like a normal push, but checks that the previous record
     * isn't in progress and the new check-in isn't
     * earlier than the previous check-out.
     *
     * Returns 0 if it succeeds or -1
     */
    int push(const Record&);

    // Usual stack functions
    Record pop();

    const Record& top() const;

    const Record& at(int) const;

    int size() const;

    bool isEmpty() const;

    /**
     * Replace the record at position i
     *
     * Returns -1 if the record's data is incompatible with the rest,
     * or if the index is out of range
     */
    int replace(int, const Record&);

    void clear();

    friend QDataStream& operator<< (QDataStream&, const RecordStack&);
    friend QDataStream& operator>> (QDataStream&, RecordStack&);

private:

    QStack<Record> m_stack;
};

QDataStream& operator<< (QDataStream&, const RecordStack&);
QDataStream& operator>> (QDataStream&, RecordStack&);

#endif // RECORDSTACK_H
