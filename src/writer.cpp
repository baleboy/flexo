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
#include <QFile>
#include <QtDebug>

#include "worker.h"
#include "writer.h"

Writer::Writer(Worker& w)
    : m_worker(w)
{
    m_xml.setAutoFormatting(true);
}

bool Writer::writeFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open savefile for writing";
        return false;
    }

    m_xml.setDevice(&file);

    m_xml.writeStartDocument();
    m_xml.writeStartElement("data");
    m_xml.writeAttribute("version", "1.0");

    m_xml.writeStartElement("workdayLength");
    m_xml.writeCharacters(QString::number(m_worker.workdayLength()));
    m_xml.writeEndElement();

    m_xml.writeStartElement("history");
    QDate currentDay;
    for (int i = 0; i < m_worker.records(); ++i) {
        if (currentDay != m_worker.checkinAt(i).date()) {
            if (currentDay.isValid())
                m_xml.writeEndElement();
            currentDay = m_worker.checkinAt(i).date();
            m_xml.writeStartElement("record");
            m_xml.writeAttribute("date", currentDay.toString(Qt::ISODate));
        }
        writeRecord(i);
    }
    m_xml.writeEndElement();
    m_xml.writeEndElement();

    m_xml.writeStartElement("balance");
    m_xml.writeCharacters(QString::number(m_worker.balance()));
    m_xml.writeEndElement();

    m_xml.writeEndDocument();
    return true;
}

void Writer::writeRecord(int i)
{
    m_xml.writeStartElement("checkin");
    m_xml.writeCharacters(m_worker.checkinAt(i).toString(Qt::ISODate));
    m_xml.writeEndElement();
    m_xml.writeStartElement("checkout");
    m_xml.writeCharacters(m_worker.checkoutAt(i).toString(Qt::ISODate));
    m_xml.writeEndElement();
}
