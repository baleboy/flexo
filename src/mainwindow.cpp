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


#include <QTime>
#include <QtGui>
#include <QSettings>
#include <QtDebug>
#include <assert.h>
#include <QMaemo5InformationBox>
#include <QDBusInterface>
#include <QMaemo5TimePickSelector>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commands.h"
#include "exitdialog.h"
#include "testalarm.h"
#include "maemo5alarm.h"
#include "balancewindow.h"
#include "preferenceswindow.h"
#include "constants.h"
#include "preferences.h"

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent), alarm(0), m_balanceWindow(0)
{
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setAttribute(Qt::WA_Maemo5StackedWindow);
    QMaemo5TimePickSelector *sel = new QMaemo5TimePickSelector;
    checkInText->setPickSelector(sel);
    checkInText->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    // connect(sel, SIGNAL(selected(QString)), this, SLOT(updateTime()));
    connect(checkInText, SIGNAL(clicked()), this, SLOT(updateTime()));

    QAction *actionReset = new QAction(tr("Clear All"), this);
    menuBar()->addAction(actionReset);
    connect(actionReset, SIGNAL(triggered()), this, SLOT(reset()));

    undoStack = new QUndoStack();
    createUndoActions();

    timer = new QTimer(this);
    timer->setInterval(TIMER_VALUE_IN_MSEC);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    m_preferences = new Preferences();

    restore();

    if (m_preferences->useAlarm()) {
        alarm = createAlarm();
    }

    qDebug() << "about to update view";
    updateView();
    qDebug() << "view updated";

    if (worker.isWorking())
        timer->start();

    this->show();

    qDebug() << "Datafile: " << SAVEFILE;
}

void MainWindow::updateView()
{
    QMaemo5TimePickSelector *sel =
            qobject_cast<QMaemo5TimePickSelector *>(checkInText->pickSelector());

    bool showText = false;

    if (worker.isWorking()) {
        checkInToggle->setChecked(true);
        if (worker.lastCheckin()) {
            checkInText->setText("Checked in at");
            sel->setCurrentTime(worker.lastCheckin()->time());
            showText = true;
        }
    }
    else {
        checkInToggle->setChecked(false);
        if (worker.lastCheckout()){
            checkInText->setText("Checked out at");
            sel->setCurrentTime(worker.lastCheckout()->time());
            showText = true;
        }
    }

    if (showText) {
        checkInText->show();
    }
    else {
        checkInText->hide();
    }
    displayTimeAtWork();
}

void MainWindow::on_checkInToggle_clicked()
{
    if (!worker.isWorking()) {
        QUndoCommand* checkinCommand = new CheckinCommand(worker, this, timer);
        undoStack->push(checkinCommand);
    }
    else {
        QUndoCommand* checkoutCommand = new CheckoutCommand(worker, this, timer);
        undoStack->push(checkoutCommand);
    }
}


void MainWindow::onTimer()
{		
        displayTimeAtWork();
}

void MainWindow::displayTimeAtWork()
{
	QString s;

        int todo = worker.isHoliday() ? 0 : worker.workdayLength();
        int done = abs(todo - worker.workDoneToday() - worker.workInProgress());

        qDebug() << "done: " << done;

        // TODO: add some checks to avoid repeating this at every tick
        if (!worker.isWorking()) {
            elapsedTimeText->setStyleSheet("color: gray");
        }
        else if (worker.isOvertime()){
            elapsedTimeText->setStyleSheet("color: red");
        }
        else {
            elapsedTimeText->setStyleSheet("color: white");
        }
        s.sprintf("%02d:%02d", done / 3600, (done % 3600) / 60);
        elapsedTimeText->setText(s);
}

void MainWindow::reset()
{
    qDebug() << "Reset";

    undoStack->push(new ClearAllCommand(this, worker, timer));
}



void MainWindow::on_balanceButton_clicked()
{
    if (!m_balanceWindow) {
        m_balanceWindow = new BalanceWindow(undoStack, &worker, m_preferences, this);
        m_balanceWindow->setAttribute(Qt::WA_Maemo5StackedWindow);
        m_balanceWindow->setWindowFlags(m_balanceWindow->windowFlags() | Qt::Window);

        connect(timer, SIGNAL(timeout()), m_balanceWindow, SLOT(showBalance()));
    }
    m_balanceWindow->show();
}

void MainWindow::on_settingsButton_clicked()
{
    if (!m_preferencesWindow) {
        m_preferencesWindow = new PreferencesWindow(m_preferences, &worker, this);
        m_preferencesWindow->setAttribute(Qt::WA_Maemo5StackedWindow);
        m_preferencesWindow->setWindowFlags(m_preferencesWindow->windowFlags() | Qt::Window);

        connect(m_preferencesWindow, SIGNAL(alarmToggled(bool)),
                this, SLOT(toggleAlarm(bool)));
        connect(m_preferencesWindow, SIGNAL(workdayUpdated()),
                this, SLOT(updateView()));
        connect(m_preferencesWindow, SIGNAL(workdayUpdated()),
                this, SLOT(setAlarm()));
    }
    m_preferencesWindow->show();
}

void MainWindow::save()
{
    m_preferences->save();
    QFile file(SAVEFILE);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Couldn't open savefile";
        return;
    }
    QDataStream out(&file);
    out << worker;

    qDebug() << worker.print();
}

void MainWindow::restore()
{
    m_preferences->load();

    QFile file(SAVEFILE);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "restore: no datafile";
        worker.setWorkdayLength(DEFAULT_WORKDAY);
    }
    else {
        QDataStream in(&file);
        in >> worker;
    }
    qDebug() << worker.print();
}

// If the user is checked in and wants to be asked what to do on exit, show the dialog,
// preselecting the option that was chosen last time. Finally save the user choices for
// the next time.
void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "Closing...";
    if (worker.isWorking()) {
        if (m_preferences->exitOption() == Preferences::onExitAsk) {
            ExitDialog dialog(this);
            if (m_preferences->defaultExitDialogOption() ==
                Preferences::onExitCheckout) {
                dialog.checkoutOption->setChecked(true);
            }
            else {
                dialog.checkinOption->setChecked(true);
            }
            if (dialog.exec()) {
                int choice;
                if (dialog.checkoutOption->isChecked()) {
                    on_checkInToggle_clicked();
                    choice = Preferences::onExitCheckout;
                }
                else {
                    choice = Preferences::onExitStayCheckedIn;
                }
                if (dialog.checkBox->isChecked()) {
                    m_preferences->setExitOption(choice);
                }
                m_preferences->setDefaultExitDialogOption(choice);
            }
            else {
                event->ignore();
                return;
            }
        }
        else if (m_preferences->exitOption() ==
                 Preferences::onExitCheckout) {
            on_checkInToggle_clicked();
        }
    }
    event->accept();
    save();
}

void MainWindow::createUndoActions()
{
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    menuBar()->addAction(undoAction);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    menuBar()->addAction(redoAction);
}

void MainWindow::setAlarm()
{
    if (alarm && worker.isWorking()) {
        if (!worker.isOvertime()) {
            QDateTime alarmTime = worker.lastCheckin()->addSecs(worker.workdayLength() - worker.workDoneToday());
            alarm->setTime(alarmTime);
            alarm->set();
        }
    }
}

void MainWindow::removeAlarm()
{
    if (alarm) alarm->remove();
}

void MainWindow::Activate()
{
    // unlock screen if allowed
    if (m_preferences->unlockScreen()) {
        QDBusInterface mce( "com.nokia.mce", "/com/nokia/mce/request",
                              "com.nokia.mce.request", QDBusConnection::systemBus() );
        mce.call( "req_tklock_mode_change", "unlocked");
    }

    // bring window to the foreground
    activateWindow();
}

void MainWindow::toggleAlarm(bool b)
{
    if (worker.isWorking()) {
        if (b) {
            setAlarm();
        }
        else {
            removeAlarm();
        }
    }
}

Alarm* MainWindow::createAlarm()
{
#ifdef Q_WS_MAEMO_5
    Maemo5Alarm* a;

    QList<alarm_id_t> alarmIds;
    Maemo5Alarm::getAlarms(APP_ID, alarmIds);

    assert(alarmIds.count() <=1);

    if (alarmIds.count() == 1) {
        a = new Maemo5Alarm(alarmIds.first());
        qDebug() << "existing alarm loaded, id " << alarmIds.first();
    }
    else {
        assert(!(worker.isWorking() && !worker.isOvertime())|| !m_preferences->useAlarm());
        a = new Maemo5Alarm(APP_ID);
        a->addDismissAction("Dismiss");
        a->addSnoozeAction("Remind me later");
        a->addDBusAction("Go to Checkout", APP_ID, APP_ID,
                         DBUS_PATH, "Activate");
    }
#else
    TestAlarm* a;
    a = new TestAlarm();
#endif
    a->setMessage("It's time to go home!");

    return a;
}

void MainWindow::updateTime()
{
    QMaemo5TimePickSelector *sel =
            qobject_cast<QMaemo5TimePickSelector *>(checkInText->pickSelector());
    QTime storedTime;
    QDateTime newDateTime;
    const QDateTime* target;

    if(worker.isWorking())
        target = worker.lastCheckin();
    else
        target = worker.lastCheckout();

    storedTime = target->time();

    if (sel->currentTime() == storedTime)
        return;

    newDateTime = *target;
    newDateTime.setTime(sel->currentTime());

    if (newDateTime > QDateTime::currentDateTime()) {
        showWarning("New time cannot be in the future");
        sel->setCurrentTime(storedTime);
        return;
    }

    if (worker.isWorking()) {
        if (worker.lastCheckout()) {
            if (newDateTime < *worker.lastCheckout()) {
                showWarning("Checkin time cannot be earlier than last checkout");
                sel->setCurrentTime(storedTime);
                return;
            }
        }
        undoStack->push(new UpdateCheckinTimeCommand(this, newDateTime, worker));
    }
    else {
        if (newDateTime < *worker.lastCheckin()) {
            showWarning("Checkout time cannot be earlier than last checkin");
            sel->setCurrentTime(storedTime);
            return;
        }
        undoStack->push(new UpdateCheckoutTimeCommand(this, newDateTime, worker));
    }
}

void MainWindow::showWarning(const QString &msg)
{
    QMaemo5InformationBox::information(this, msg);
}
