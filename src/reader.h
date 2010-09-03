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

#ifndef READER_H
#define READER_H

#include <QXmlStreamReader>

class Worker;
class QIODevice;

class Reader
{
public:
    Reader(Worker&);

    bool readFile(const QString&);
    void checkForLegacyData();

    QString errorString() const;

private:

    Worker& m_worker;
    QXmlStreamReader m_xml;

    void readData();
    void readWorkdayLength();
    void readHistory();
    void readRecord();
    void readLegacyData1();
    void readLegacyData2();
    void readBalance();
    QString backupFileName(const QString& s);
};

#endif // READER_H
