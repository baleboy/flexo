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


#include <QtTest/QtTest>
#include "record.h"
#include "recordstack.h"
#include "constants.h"

class TestRecordStack: public QObject
{
    Q_OBJECT
private slots:

    void init();

    void testPush();
    void testReplace();
};

void TestRecordStack::init()
{
    clock_ = QDateTime::fromString("M2d12y201011:01:02",
                                   "'M'M'd'd'y'yyyyhh:mm:ss");
}

void TestRecordStack::testPush()
{
    RecordStack s;
    QVERIFY(s.push(Record(CURRENT_TIME)) == 0);
    QVERIFY(s.push(Record(CURRENT_TIME)) == -1);
    s.clear();

    Record r(CURRENT_TIME);
    int delta = 200;

    r.setCheckoutTime(CURRENT_TIME.addSecs(delta));
    QVERIFY(s.push(r) == 0);
    QVERIFY(s.push(Record(CURRENT_TIME)) == -1);
    QVERIFY(s.push(Record(CURRENT_TIME.addSecs(delta))) == 0);
}

void TestRecordStack::testReplace()
{
}


QTEST_MAIN(TestRecordStack)
#include "testrecordstack.moc"
