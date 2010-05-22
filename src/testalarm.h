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

#ifndef TESTALARM_H
#define TESTALARM_H

#include "alarm.h"

/*!
 * Implementation of the alarm interface for testing purposes. Meant to be used
 * when compiling for the desktop.
 */
class TestAlarm : public Alarm
{
public:
    TestAlarm();

    void set();
    void remove();
    bool isSet() const { return m_set; };

private:

    bool m_set;
};

#endif // TESTALARM_H
