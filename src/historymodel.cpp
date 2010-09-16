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

#include "historymodel.h"
#include "historydelegate.h"

HistoryModel::HistoryModel(Worker* worker, QObject *parent) :
    QAbstractListModel(parent), m_worker(worker)
{
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return numberOfDays();
}

int HistoryModel::numberOfDays() const
{
    QDate day;
    int count = 0;

    for (int i = 0 ; i < m_worker->records() ; ++i) {
        if (day != m_worker->checkinAt(i).date()) {
            day = m_worker->checkinAt(i).date();
            count++;
        }
    }

    return count;
}

int HistoryModel::indexOfDay(int day) const
{
    if (m_worker->records() == 0)
        return 0;

    int dayIndex = 0;
    int i = 0;
    QDate date = m_worker->checkinAt(0).date();

    while (dayIndex != day) {
        if (date != m_worker->checkinAt(i).date()) {
            if (++dayIndex == day)
                break;
            date = m_worker->checkinAt(i).date();
        }
        ++i;
    }
    return i;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= numberOfDays() || index.row() < 0)
        return QVariant();

    QLocale locale(QLocale::English);

    switch (role) {
    case Qt::DisplayRole:
    case HistoryDelegate::MainText1Role:
        return locale.toString(m_worker->checkinAt(indexOfDay(index.row())).date(), "dddd MMM dd yyyy");

    case HistoryDelegate::MainText2Role: {
            int i = indexOfDay(index.row());
            int balance = 0;
            QDate day = m_worker->checkinAt(indexOfDay(index.row())).date();
            while (i < m_worker->records() && (day == m_worker->checkinAt(i).date())) {
                if (m_worker->checkoutAt(i).isValid())
                    balance += m_worker->checkinAt(i).secsTo(m_worker->checkoutAt(i));
                ++i;
            }
            return (balance > m_worker->workdayLength()? "+" : "") +
                    QString::number(double(balance - m_worker->workdayLength()) / 3600.0,
                                    'f', 1);
        }

    case HistoryDelegate::SubText1Role: {
            int i = indexOfDay(index.row());
            QDate day = m_worker->checkinAt(indexOfDay(index.row())).date();
            while (i < m_worker->records() && (day == m_worker->checkinAt(i).date())) {
                ++i;
            }
            if (i != 0)
                --i;
            return  "Checked in at " +
                    m_worker->checkinAt(indexOfDay(index.row())).time().toString("hh:mm")
                    + ", checked out at " + m_worker->checkoutAt(i).time().toString("hh:mm");
        }
    default:
        return QVariant();

    }

}


