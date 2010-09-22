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

#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <QLocale>

#include "worker.h"

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit HistoryModel(QObject *parent = 0);

    HistoryModel(Worker* worker, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    enum { dayMode, weekMode, monthMode };

public slots:
    void setMode(int m);

private:

   Worker* m_worker;
   int m_mode;

   int indexOfPeriod(int period) const;
   int numberOfPeriods() const;
   int periodByMode(int i) const;
   QString dateByMode(const QDateTime& dateTime) const;

   static QLocale m_locale;
};

#endif // TABLEMODEL_H
