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
#include "constants.h"

class TestRecord: public QObject
{
    Q_OBJECT
private slots:

    void init();

    void testCheckin();
    void testCheckout();
    void testElapsed();
    void testInvalidTimes();
    void testSerialize();
};

void TestRecord::init()
{
    clock_ = QDateTime::fromString("M2d12y201011:01:02",
                                   "'M'M'd'd'y'yyyyhh:mm:ss");
}

void TestRecord::testCheckin()
{
    QDateTime t = CURRENT_TIME;
    Record r(t);
    int delta = 1000;

    int res = r.setCheckinTime(t.addSecs(-delta));
    QVERIFY (res == 0);
    QCOMPARE (r.checkinTime(), t.addSecs(-delta));

    res = r.setCheckinTime(t.addSecs(delta));

    QVERIFY (res == 0);
    QCOMPARE (r.checkinTime(), t.addSecs(delta));
}

void TestRecord::testCheckout()
{
    QDateTime t = CURRENT_TIME;
    Record r(t);
    int delta = 1000;

    int res = r.setCheckoutTime(t.addSecs(-delta));
    QVERIFY (res == -1);
    QVERIFY (r.inProgress());

    res = r.setCheckoutTime(t.addSecs(delta));

    QVERIFY (res == 0);
    QCOMPARE (r.checkoutTime(), t.addSecs(delta));
    QVERIFY (!r.inProgress());
}

void TestRecord::testElapsed()
{
    Record r(CURRENT_TIME);

    int delta1 = 4000;
    int delta2 = 6000;

    clock_ = clock_.addSecs(delta1);

    QCOMPARE(r.elapsed(), delta1);
    QVERIFY(r.inProgress());

    clock_ = clock_.addSecs(delta2);
    r.setCheckoutTime(CURRENT_TIME);

    QVERIFY(!r.inProgress());
    QCOMPARE(r.elapsed(), delta1 + delta2);
}

void TestRecord::testInvalidTimes()
{
    QDateTime t = CURRENT_TIME;

    Record r(t);

    int delta = 4000;

    int res = r.setCheckoutTime(t.addSecs(delta));
    QVERIFY (res == 0);

    res = r.setCheckinTime(t.addSecs(delta*2));
    QVERIFY (res == -1);
    QCOMPARE (r.checkinTime(), t);
}

void TestRecord::testSerialize()
{
    Record r1(CURRENT_TIME);
    r1.setCheckoutTime(CURRENT_TIME.addSecs(1000));

    QBuffer b;
    b.open(QIODevice::WriteOnly);
    QDataStream stream(&b);
    stream << r1;
    b.close();

    b.open(QIODevice::ReadOnly);
    Record r2;
    stream >> r2;
    b.close();

    QCOMPARE(r1.checkinTime(), r2.checkinTime());
    QCOMPARE(r1.checkoutTime(), r2.checkoutTime());
    QCOMPARE(r1.elapsed(), r2.elapsed());
    QCOMPARE(r1.inProgress(), r2.inProgress());
}

QTEST_MAIN(TestRecord)
#include "testrecord.moc"
