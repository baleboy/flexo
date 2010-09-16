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

#include <QPainter>
#include <QFontMetrics>

#include "historydelegate.h"

HistoryDelegate::HistoryDelegate(QObject *parent)
{
}


void HistoryDelegate:: paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    painter->save();

    painter->setFont(QFont("Arial", 20));

    QString txt = index.data(MainText1Role).toString();

    painter->drawText(option.rect.x() + 20, option.rect.y() + 35, txt);

    txt = index.data(MainText2Role).toString();

    QFont bigFont("Arial", 30);
    QFontMetrics fm(bigFont);
    int textWidth = fm.width(txt);
    painter->setFont(bigFont);

    painter->drawText(option.rect.x() + 730 - textWidth, option.rect.y() + 45, txt);

    painter->setPen(Qt::gray);
    painter->setFont(QFont("Arial", 15));
    txt = index.data(SubText1Role).toString();
    painter->drawText(option.rect.x() + 20, option.rect.y() + 65, txt);

    txt = index.data(SubText2Role).toString();
    painter->drawText(option.rect.x() + 300, option.rect.y() + 55, txt);

    painter->restore();
}

QSize HistoryDelegate::sizeHint(const QStyleOptionViewItem &option,
               const QModelIndex &index ) const
{
    // return QSize(option.rect.width(), option.rect.height());
    // return QSize(20, 300);
    return QSize(option.rect.width(), 80);
}
