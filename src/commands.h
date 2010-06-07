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

class MainWindow;
class QTimer;

/*!
 * Redo and Undo actions for the check in command.
 *
 * This command checks in the worker and sets an alarm if needed. Undo restores
 * all the data as it was before the command, and removed the alarm.
 */
class CheckinCommand : public QUndoCommand
{

public:

    CheckinCommand(Worker&, MainWindow*, QTimer*);
    void undo();
    void redo();

private:

    Worker& worker_;
    MainWindow* ui_;
    QTimer* timer_;
    bool firstRedo_;
    Worker oldWorker_;
    Worker checkedInWorker_;
};

/*!
 * Redo and Undo actions for the check out command
 *
 * This command checks in the worker and removes any pending alarm. Undo restores
 * all the data as it was before the command, and re-sets the alarm.
 */
class CheckoutCommand : public QUndoCommand
{

public:

    CheckoutCommand(Worker&, MainWindow*, QTimer*);
    void undo();
    void redo();

private:

    Worker& worker_;
    MainWindow* ui_;
    QTimer* timer_;
    bool firstRedo_;
    Worker oldWorker_;
    Worker checkedOutWorker_;

};

/*!
 * Redo and Undo actions for the "edit balance" command (triggered when the
 * balance in the balance spin box is edited)
 */
class EditBalanceCommand : public QUndoCommand
{

public:

    EditBalanceCommand(BalanceWindow*, int newBalance, Worker*);
    void undo();
    void redo();

private:

    BalanceWindow* m_window;
    int m_newBalance;
    int m_oldBalance;
    Worker *m_worker;
};

/*!
 * Redo and Undo actions for the "clear all" command
 */
class ClearAllCommand : public QUndoCommand
{
public:
    ClearAllCommand(MainWindow*, Worker&, QTimer*);
    void undo();
    void redo();

private:

    MainWindow* ui_;
    Worker& worker_;
    Worker oldWorker_;
    QTimer* timer_;

};

#endif // COMMANDS_H
