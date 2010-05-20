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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>

#include "worker.h"

class Timecard;
class QTimer;

class CheckinCommand : public QUndoCommand
{

public:

    CheckinCommand(Worker&, Timecard*, QTimer*);
    void undo();
    void redo();

private:

    Worker& worker_;
    Timecard* ui_;
    QTimer* timer_;
    bool firstRedo_;
    Worker oldWorker_;
    Worker checkedInWorker_;
};

class CheckoutCommand : public QUndoCommand
{

public:

    CheckoutCommand(Worker&, Timecard*, QTimer*);
    void undo();
    void redo();

private:

    Worker& worker_;
    Timecard* ui_;
    QTimer* timer_;
    bool firstRedo_;
    Worker oldWorker_;
    Worker checkedOutWorker_;

};

class EditBalanceCommand : public QUndoCommand
{

public:

    EditBalanceCommand(Timecard*, int, Worker&);
    void undo();
    void redo();

private:

    Timecard* ui_;
    int newBalance_;
    int oldBalance_;
    Worker& worker_;
};

class ClearAllCommand : public QUndoCommand
{
public:
    ClearAllCommand(Timecard*, Worker&, QTimer*);
    void undo();
    void redo();

private:

    Timecard* ui_;
    Worker& worker_;
    Worker oldWorker_;
    QTimer* timer_;

};

#endif // COMMANDS_H
