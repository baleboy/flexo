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

QLocale HistoryModel::m_locale(QLocale::English);

HistoryModel::HistoryModel(Worker* worker, QObject *parent) :
        QAbstractListModel(parent), m_worker(worker), m_mode(dayMode)
{
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return numberOfPeriods();
}

void HistoryModel::setMode(int m)
{
    m_mode = m;
    emit layoutChanged();
}

int HistoryModel::periodByMode(int i) const
{
    int p;
    switch (m_mode) {
    case dayMode:
        p = m_worker->checkinAt(i).date().day();
        break;
    case weekMode:
        p = m_worker->checkinAt(i).date().weekNumber();
        break;
    case monthMode:
        p = m_worker->checkinAt(i).date().month();
        break;
    }
    return p;
}
int HistoryModel::numberOfPeriods() const
{
    int lastPeriod = -1;
    int count = 0;

    for (int i = 0 ; i < m_worker->records() ; ++i) {
        int currentPeriod = periodByMode(i);
        if (lastPeriod != currentPeriod) {
            lastPeriod = currentPeriod;
            count++;
        }
    }
    return count;
}


int HistoryModel::indexOfPeriod(int p) const
{
    if (m_worker->records() == 0)
        return 0;

    int periodIndex = 0;
    int i = 0;
    int lastPeriod = periodByMode(0);

    while (periodIndex != p) {
        if (lastPeriod != periodByMode(i)) {
            if (++periodIndex == p)
                break;
            lastPeriod = periodByMode(i);
        }
        ++i;
    }
    return i;
}

QString HistoryModel::dateByMode(const QDateTime& t) const
{
    QString text;
    switch (m_mode) {
    case dayMode:
        text = t.time().toString("hh:mm");
        break;
    case weekMode:
        text = m_locale.toString(t.date(), "MMM dd");
        break;
    case monthMode:
        text = m_locale.toString(t.date(), "ddd dd");
    }
    return text;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= numberOfPeriods() || index.row() < 0)
        return QVariant();

    int reverseIndex = numberOfPeriods() - index.row() - 1;

    switch (role) {
    case Qt::DisplayRole:
    case HistoryDelegate::MainTextRole: {
            QString txt;
            switch (m_mode) {
            case dayMode:
                txt = m_locale.toString(m_worker->checkinAt(indexOfPeriod(reverseIndex)).date(), "dddd MMM dd yyyy");
                break;
            case weekMode:
                txt = "Week " +
                      QString::number(m_worker->checkinAt(indexOfPeriod(reverseIndex)).date().weekNumber()) +
                      + ", " + m_locale.toString(m_worker->checkinAt(indexOfPeriod(reverseIndex)).date(), "yyyy");
                break;
            case monthMode:
                txt = m_locale.toString(m_worker->checkinAt(indexOfPeriod(reverseIndex)).date(), "MMMM yyyy");
                break;
            }

            return txt;
        }

    case HistoryDelegate::NumberRole: {
            int i = indexOfPeriod(reverseIndex);
            QDate day = m_worker->checkinAt(i).date();

            int actual = 0;
            int wanted = Worker::isHoliday(day) ? 0 : m_worker->workdayLength();
            int currentPeriod = periodByMode(indexOfPeriod(reverseIndex));
            while (i < m_worker->records() && (currentPeriod == periodByMode(i))) {
                if (day != m_worker->checkinAt(i).date()) {
                    day = m_worker->checkinAt(i).date();
                    wanted += Worker::isHoliday(day) ? 0 : m_worker->workdayLength();
                }
                if (m_worker->checkoutAt(i).isValid())
                    actual += m_worker->checkinAt(i).secsTo(m_worker->checkoutAt(i));
                ++i;
            }
            return double(actual - wanted) / 3600.0;
        }

    case HistoryDelegate::SubTextRole: {
            int i = indexOfPeriod(reverseIndex);
            int p = i;
            int currentPeriod = periodByMode(p);
            while (i < m_worker->records() && (currentPeriod == periodByMode(i))) {
                ++i;
            }
            if (i != 0)
                --i;
            QString text = "First check-in: " + dateByMode(m_worker->checkinAt(p));

            if (m_worker->checkoutAt(i).isValid())
                text += "  Last check-out: " + dateByMode(m_worker->checkoutAt(i));

            return text;
        }
    default:
        return QVariant();

    }

}


