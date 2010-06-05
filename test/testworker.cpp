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

#include <QtTest/QtTest>
#include "worker.h"

class TestWorker: public QObject
{
    Q_OBJECT
private slots:

    void testConstructor();
    void testCopy();
    void testWorkInProgress();
    void testBalancePositive();
    void testBalanceNegative();
    void testShortBalance();
    void testDayChange();
    void testNewDay();
    void testWithRealClock();
    void testOvertime();
    void testNeverCheckedin();
    void testSerialization();
    void testPrint();
};

void TestWorker::testConstructor()
{
    Worker out;
    clock_ = QDateTime::fromString("8:30", "hh:mm");

    QVERIFY(out.workDoneToday() == 0);
    QVERIFY(!out.isWorking());
    QVERIFY(out.workdayLength() == 3600*7.5);

    out.setBalance(5);
    out.checkin();

    clock_ = clock_.addSecs(7245);

    out.checkout();

    Worker w(out);

    QVERIFY(w.balance() == out.balance());
    QVERIFY(*w.lastCheckin() == *out.lastCheckin());
    QVERIFY(*w.lastCheckout() == *out.lastCheckout());
    QVERIFY(w.workDoneToday() == out.workDoneToday());
    QVERIFY(w.isWorking() == w.isWorking());
    QVERIFY(w.workInProgress() == w.workInProgress());

}

void TestWorker::testCopy()
{
    Worker w1, w2;

    w1 = w2;

    w1.setBalance(10000);
    w1.setWorkdayLength(27000);
    w1.setWorkDoneToday(4000);
    w1.checkin();

    w2 = w1;

    QVERIFY(w1.balance() == w2.balance());
    QVERIFY(*w1.lastCheckin() == *w2.lastCheckin());
    QVERIFY((w1.lastCheckout() == w2.lastCheckout()) && !w2.lastCheckout());
    QVERIFY(w1.workDoneToday() == w2.workDoneToday());
    QVERIFY(w1.isWorking() == w2.isWorking());
    QVERIFY(w1.workInProgress() == w2.workInProgress());

    w1.checkout();
    w2 = w1;

    QVERIFY(w1.balance() == w2.balance());
    QVERIFY(*w1.lastCheckin() == *w2.lastCheckin());
    QVERIFY(*w1.lastCheckout() == *w2.lastCheckout());
    QVERIFY(w1.workDoneToday() == w2.workDoneToday());
    QVERIFY(w1.isWorking() == w2.isWorking());
    QVERIFY(w1.workInProgress() == w2.workInProgress());
}

void TestWorker::testWorkInProgress()
{
    Worker out;
    clock_ = QDateTime::fromString("8:30", "hh:mm");

    QCOMPARE(out.checkin(), clock_);
    QCOMPARE(*out.lastCheckin(), clock_);
    QVERIFY(out.isWorking());

    clock_ = clock_.addSecs(7245);

    QVERIFY(out.workInProgress() == 7245);
    QVERIFY(out.workDoneToday() == 0);

    out.checkout();

    QVERIFY(!out.isWorking());
    QVERIFY(out.workInProgress() == 0);
    QVERIFY(out.workDoneToday() == 7245);

    clock_= clock_.addSecs(1000);

    out.checkin();
    QVERIFY(out.workInProgress() == 0);
    QVERIFY(out.workDoneToday() == 7245);

    clock_ = clock_.addSecs(1000);

    QVERIFY(out.workInProgress() == 1000);
    QVERIFY(out.workDoneToday() == 7245);

    out.checkout();
    QVERIFY(out.workInProgress() == 0);
    QVERIFY(out.workDoneToday() == 8245);

}

void TestWorker::testBalancePositive()
{
    int workday = 3600*7.5;
    int worked = 3600*8;

    Worker out;
    out.setWorkdayLength(workday);
    clock_ = QDateTime::fromString("8:30", "hh:mm");

    out.checkin();
    clock_=clock_.addSecs(worked);

    QCOMPARE(out.balance(), -workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

    out.checkout();

    QCOMPARE(out.balance(), worked-workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

}

void TestWorker::testBalanceNegative()
{
    int workday = 3600*7.5;
    int worked = 3600*7;

    Worker out;
    out.setWorkdayLength(workday);
    clock_ = QDateTime::fromString("8:30", "hh:mm");

    out.checkin();
    clock_=clock_.addSecs(worked);

    QCOMPARE(out.balance(), -workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

    out.checkout();

    QCOMPARE(out.balance(), worked-workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

}

void TestWorker::testShortBalance()
{
    int workday = 3600*7.5;
    int worked = 20;

    Worker out;
    out.setWorkdayLength(workday);
    clock_ = QDateTime::fromString("M1d1y0911:01:02",
                                   "'M'M'd'd'y'yyhh:mm:ss");


    out.checkin();
    clock_=clock_.addSecs(worked);


    QCOMPARE(out.balance(), -workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

    out.checkout();

    QCOMPARE(out.balance(), worked-workday);
    QCOMPARE(out.balanceInProgress(), worked-workday);

}

void TestWorker::testDayChange()
{
    int workday = 3600*7.5;

    Worker out;
    out.setWorkdayLength(workday);
    QDateTime checkinTime = QDateTime::fromString("22:30", "hh:mm");
    clock_ = checkinTime;
    out.checkin();

    QDateTime checkoutTime = checkinTime.addDays(1);
    checkoutTime.setTime(QTime::fromString("09:30", "hh:mm"));
    clock_ = checkoutTime;
    out.checkout();

    QDateTime newStart = checkoutTime;
    newStart.setTime(QTime::fromString("05:00:00", "hh:mm:ss"));

    QCOMPARE(*out.lastCheckin(), newStart);
    QCOMPARE(out.balance(), (checkinTime.secsTo(checkoutTime) - 2*workday));

    out.checkin();
    QCOMPARE(out.workDoneToday(), newStart.secsTo(checkoutTime));
}

void TestWorker::testNewDay()
{
    Worker w;
    clock_ = QDateTime::fromString("09:30", "hh:mm");
    w.checkin();
    clock_ = clock_.addSecs(3600);
    w.checkout();
    clock_ = clock_.addDays(1);
    QVERIFY(w.workDoneToday() == 0);
}

void TestWorker::testWithRealClock()
{
    Worker w;
    w.setWorkdayLength(3600*7.5);

    clock_ = QDateTime::currentDateTime();
    w.checkin();
    QTest::qWait(5000);
    QDateTime previousClock = clock_;
    clock_ = QDateTime::currentDateTime();
    w.checkout();

    if (!w.isHoliday(clock_)) {
        QCOMPARE(previousClock.secsTo(clock_) - w.workdayLength(), w.balance());
    }
    else {
        QCOMPARE(previousClock.secsTo(clock_), w.balance());
    }
}

void TestWorker::testOvertime()
{
    Worker w;

    clock_ = QDateTime::fromString("M1d1y201011:01:02",
                                   "'M'M'd'd'y'yyyyhh:mm:ss");
    QVERIFY(!w.isHoliday());

    w.checkin();

    clock_ = clock_.addSecs(1000);

    QVERIFY(!w.isOvertime());

    clock_ = clock_.addSecs(w.workdayLength());

    QVERIFY(w.isOvertime());

    w.checkout();

    QVERIFY(w.isOvertime());

}

void TestWorker::testNeverCheckedin()
{
    Worker w;
    w.setBalance(200);
    QCOMPARE(w.balanceInProgress(), 200);
    QCOMPARE(w.balance(), 200);
}

void TestWorker::testSerialization()
{
    Worker w1;
    int workday = 3600*7.5;
    w1.setWorkdayLength(workday);
    QDateTime checkinTime = QDateTime::fromString("M1d1y201011:01:02",
                                                  "'M'M'd'd'y'yyyyhh:mm:ss");
    clock_ = checkinTime;

    w1.checkin();

    clock_ = clock_.addSecs(300);

    QBuffer b;
    b.open(QIODevice::WriteOnly);
    QDataStream stream(&b);
    stream << w1;
    b.close();

    b.open(QIODevice::ReadOnly);
    Worker w2;
    stream >> w2;
    b.close();

    clock_ = clock_.addSecs(300);

    qDebug() << w1.print();
    qDebug() << w2.print();

    QCOMPARE(w1.workInProgress(), w2.workInProgress());
    QCOMPARE(w1.balance(), w2.balance());
    QCOMPARE(w1.balanceInProgress(), w2.balanceInProgress());
    QCOMPARE(w1.workDoneToday(), w2.workDoneToday());
    QCOMPARE(*w1.lastCheckin(), *w2.lastCheckin());

    w1.checkout();
    b.open(QIODevice::WriteOnly);
    stream << w1;
    b.close();

    b.open(QIODevice::ReadOnly);
    stream >> w2;

    QCOMPARE(w1.workInProgress(), w2.workInProgress());
    QCOMPARE(w1.balance(), w2.balance());
    QCOMPARE(w1.balanceInProgress(), w2.balanceInProgress());
    QCOMPARE(w1.workDoneToday(), w2.workDoneToday());
    QCOMPARE(*w1.lastCheckin(), *w2.lastCheckin());
    QCOMPARE(*w1.lastCheckout(), *w2.lastCheckout());

}

void TestWorker::testPrint()
{
    Worker w;

    int workday = 3600*7.5;
    w.setWorkdayLength(workday);
    QDateTime checkinTime = QDateTime::fromString("M1d1y201011:01:02",
                                                  "'M'M'd'd'y'yyyyhh:mm:ss");
    clock_ = checkinTime;

    w.checkin();

    qDebug() << w.print();
}

QTEST_MAIN(TestWorker)
#include "testworker.moc"

