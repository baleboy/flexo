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

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QTextStream>
#include <QtDebug>

#include "mainwindow.h"
#include "balancewindow.h"
#include "commands.h"

// Check In

CheckinCommand::CheckinCommand(Worker& w, MainWindow* ui, QTimer* timer)
                                   : worker_(w),
                                     ui_(ui),
                                     timer_(timer),
                                     firstRedo_(true)
{
    oldWorker_ = w;
    setText("Check In");
}

void CheckinCommand::redo()
{
    if (firstRedo_) {
        worker_.checkin();
        checkedInWorker_ = worker_;
        firstRedo_ = false;
    }
    else {
        worker_ = checkedInWorker_;
    }
    timer_->start();
    ui_->updateView();
    ui_->setAlarm();
}

void CheckinCommand::undo()
{
    worker_ = oldWorker_;
    timer_->stop();
    ui_->updateView();
    ui_->removeAlarm();
}

// Check Out

CheckoutCommand::CheckoutCommand(Worker& w, MainWindow* ui, QTimer* timer)
    : worker_(w), ui_(ui), timer_(timer), firstRedo_(true)
{
    oldWorker_ = w;
    setText("Check Out");
}

void CheckoutCommand::redo()
{
    if (firstRedo_) {
        worker_.checkout();
        checkedOutWorker_ = worker_;
        firstRedo_ = false;
    }
    else {
        worker_ = checkedOutWorker_;
    }
    timer_->stop();
    ui_->updateView();
    ui_->removeAlarm();
}

void CheckoutCommand::undo()
{
    worker_ = oldWorker_;
    timer_->start();
    ui_->updateView();
    ui_->setAlarm();
}

// Edit Balance

EditBalanceCommand::EditBalanceCommand(BalanceWindow* w, int newBalance, Worker *worker)
    : m_window(w), m_newBalance(newBalance), m_worker(worker)
{
    setText("Edit Balance");
    m_oldBalance = m_worker->balance();
}

void EditBalanceCommand::redo()
{
    qDebug() << "Edit Balance/redo: " << m_newBalance;
    m_worker->setBalance(m_newBalance);
    m_window->showBalance();
}

void EditBalanceCommand::undo()
{
    qDebug() << "Edit Balance/undo: " << m_oldBalance;
    m_worker->setBalance(m_oldBalance);
    m_window->showBalance();
}

// Clear All

ClearAllCommand::ClearAllCommand(MainWindow* ui, Worker& w, QTimer* timer)
    : ui_(ui), worker_(w), oldWorker_(w), timer_(timer)
{
    setText("Clear All");
}

void ClearAllCommand::redo()
{
    worker_.clear();
    timer_->stop();
    ui_->updateView();
}

void ClearAllCommand::undo()
{
    worker_ = oldWorker_;
    ui_->updateView();
    if (worker_.isWorking()) {
        timer_->start();
    }
}
