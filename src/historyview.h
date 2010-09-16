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

#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include "ui_historyview.h"

class Worker;

class HistoryView : public QWidget, private Ui::HistoryView
{
    Q_OBJECT

public:
    explicit HistoryView(Worker *worker, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

};

#endif // HISTORYVIEW_H
