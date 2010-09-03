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
#include <QtDebug>
#include <QDataStream>
#include <QIODevice>
#include <QFile>
#include <QDir>

#include "reader.h"
#include "worker.h"

const QString LEGACY_SAVEFILE1 = QDir::home().filePath(".flexo");
const QString LEGACY_SAVEFILE2 = QDir::home().filePath(".flexo.data");

Reader::Reader(Worker& w)
    : m_worker(w)
{
}

bool Reader::readFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        m_xml.raiseError("File not found");
        return false;
    }

    m_xml.setDevice(&file);

    if (m_xml.readNextStartElement()) {
        if (m_xml.name() == "data" && m_xml.attributes().value("version") == "1.0")
            readData();
        else
            m_xml.raiseError("The file is not a valid Flexo datafile");
    }

    return !m_xml.error();
}

void Reader::readData()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "data");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "workdayLength")
            readWorkdayLength();
        else if (m_xml.name() == "balance")
            readBalance();
        else if (m_xml.name() == "history")
            readHistory();
        else
            m_xml.skipCurrentElement();
    }
}

void Reader::readWorkdayLength()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "workdayLength");

    m_worker.setWorkdayLength(m_xml.readElementText().toInt());
}

void Reader::readBalance()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "balance");

    m_worker.setBalance(m_xml.readElementText().toInt());
}

void Reader::readHistory()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "history");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "record")
            readRecord();
        else
            m_xml.skipCurrentElement();
    }
}

void Reader::readRecord()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "record");

    while (m_xml.readNextStartElement()) {
        qDebug() << "processing " << m_xml.name();

        if (m_xml.name() == "checkin") {
            QDateTime t = QDateTime::fromString(m_xml.readElementText(), Qt::ISODate);
            if (t.isValid()) {
                if (m_worker.checkin().isNull()) {
                    m_xml.raiseError("two checkins in a row");
                    break;
                }

                if (m_worker.updateCheckinTime(t) == -1) {
                    m_xml.raiseError("invalid checkin time");
                    break;
                }
            }
            else {
                m_xml.raiseError("empty checkin");
                break;
            }
        }
        else if (m_xml.name() == "checkout") {
            QDateTime t = QDateTime::fromString(m_xml.readElementText(), Qt::ISODate);
            if (t.isValid()) {
                if (m_worker.checkout() == -1) {
                    m_xml.raiseError("two checkouts in a row");
                    break;
                }
                if (m_worker.updateCheckoutTime(t) == -1) {
                    m_xml.raiseError("invalid checkout time");
                    break;
                }
            }
        }
        else
            m_xml.skipCurrentElement();
    }


}

QString Reader::errorString() const
{
    return QString("%1\nLine %2, column %3")
            .arg(m_xml.errorString())
            .arg(m_xml.lineNumber())
            .arg(m_xml.columnNumber());
}

void Reader::readLegacyData1()
{
    QFile file(LEGACY_SAVEFILE1);
    if (!file.open(QIODevice::ReadOnly))
        return;

    quint32 balance;
    quint8 working;
    quint32 doneToday;
    quint32 wdayLength;
    quint8 hasTime;
    QDateTime checkinTime;
    QDateTime checkoutTime;

    QDataStream in(&file);

    in >> balance >> working >> doneToday >> wdayLength >> hasTime;

    if (hasTime) {
        in >> checkinTime;
        m_worker.checkin();
        m_worker.updateCheckinTime(checkinTime);
    }
    in >> hasTime;
    if (hasTime && !working) {
        in >> checkoutTime;
        m_worker.checkout();
        m_worker.updateCheckoutTime(checkoutTime);
    }
    qDebug() << "Old balance: " << balance;
    m_worker.setWorkdayLength(wdayLength);
    m_worker.setBalance(balance);

    file.rename(backupFileName(LEGACY_SAVEFILE1));
}

void Reader::readLegacyData2()
{
    QFile file(LEGACY_SAVEFILE2);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QDataStream in(&file);
    in >> m_worker;

    file.rename(backupFileName(LEGACY_SAVEFILE2));
}

void Reader::checkForLegacyData()
{
    readLegacyData1();
    readLegacyData2();
}

QString Reader::backupFileName(const QString& s)
{
    int i = 0;
    QString curName = s + ".backup";
    while(QFile::exists(curName))
        curName = s + ".backup." + QString::number(i++);

    return curName;
}
