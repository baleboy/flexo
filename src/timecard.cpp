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

#include "timecard.h"
#include "ui_timecard.h"
#include "commands.h"
#include "exitdialog.h"
#include "testalarm.h"
#include "maemo5alarm.h"

#define DEFAULT_WORKDAY (7.5*3600)
#define TIMER_VALUE_IN_MSEC 60000
#define APP_ID "com.balenet.flexo"

enum { onExitAsk = 0, onExitCheckout = 1, onExitStayCheckedIn = 2 };

Timecard::Timecard(QWidget* parent)	
        : QMainWindow(parent), alarm(0)
{
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    QAction *actionReset = new QAction(tr("Clear All"), this);
    menuBar()->addAction(actionReset);
    connect(actionReset, SIGNAL(triggered()), this, SLOT(reset()));

    undoStack = new QUndoStack();
    createUndoActions();

    toolbarGroup = new QActionGroup(this);
    toolbarGroup->addAction(actionClock);
    toolbarGroup->addAction(actionBalance);
    toolbarGroup->addAction(actionSettings);
    actionClock->setChecked(true);

    timer = new QTimer(this);
    timer->setInterval(TIMER_VALUE_IN_MSEC);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    restore();
    balanceUnitSelector->setCurrentIndex(0);
    showBalanceInHours = true;

    alarm = createAlarm();

    updateView();

    this->show();
}

void Timecard::updateView()
{
    QString textBuffer;
    QTextStream out(&textBuffer);
    bool showText = false;

    if (worker.isWorking()) {
        checkInToggle->setChecked(true);
        balanceEdit->setEnabled(false);
        if (worker.lastCheckin()) {
            out << "Checked in at " <<
                    worker.lastCheckin()->time().toString("hh:mm");
            showText = true;
        }
    }
    else {
        checkInToggle->setChecked(false);
        balanceEdit->setEnabled(true);
        if (worker.lastCheckout()){
            out << "Checked out at " <<
                    worker.lastCheckout()->time().toString("hh:mm");
            showText = true;
        }
    }

    if (showText) {
        checkInText->setText(textBuffer);
    }
    else {
        checkInText->setText("");
    }
    displayTimeAtWork();
    displayBalance();
}

void Timecard::on_checkInToggle_clicked()
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


void Timecard::onTimer()
{		
        displayTimeAtWork();
        displayBalance();
}

void Timecard::on_balanceEdit_editingFinished()
{
        qDebug() << "Editing finished";
	
	double b = balanceEdit->text().toDouble();
        int balance;
	
	if (showBalanceInHours) {
		balance = b * 3600;
	}
	else {
                balance = b * worker.workdayLength();
	}
        if (balance != worker.balance()) {
           undoStack->push(new EditBalanceCommand(this, balance, worker));
        }
}

void Timecard::on_balanceUnitSelector_currentIndexChanged(int index)
{
        qDebug() << "Index changed: " << index;
	showBalanceInHours = (index == 0);
	displayBalance();
}

void Timecard::on_actionClock_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(clockPage);
        setWindowTitle(actionClock->text());
    }
}

void Timecard::on_actionBalance_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(balancePage);
        setWindowTitle(actionBalance->text());
    }
}

void Timecard::on_actionSettings_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(settingsPage);
        setWindowTitle(actionSettings->text());
    }
}

void Timecard::displayTimeAtWork()
{
	QString s;
        int done = worker.workDoneToday() + worker.workInProgress();
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

void Timecard::displayBalance()
{	
        double b = worker.balanceInProgress();
	if (showBalanceInHours) {
                b = b / 3600;
	}
	else {
                b = b / worker.workdayLength();
	}
        balanceEdit->setValue(b);
}

void Timecard::reset()
{
    qDebug() << "Reset";

    undoStack->push(new ClearAllCommand(this, worker, timer));

}

void Timecard::on_exitOptionComboBox_currentIndexChanged(int option)
{
    exitOption = option;
}

void Timecard::on_workdaySpinBox_editingFinished()
{
    worker.setWorkdayLength(workdaySpinBox->value() * 3600);
    updateView();
}

void Timecard::save()
{
    QSettings settings("balenet", "Timecard");

    settings.setValue("balance", worker.balance());
    settings.setValue("working", worker.isWorking());
    settings.setValue("workDoneToday", worker.workDoneToday());
    settings.setValue("workdayLength", worker.workdayLength());
    settings.setValue("useAlarm", useAlarm);

    if (worker.lastCheckin()) {
        settings.setValue("hasCheckedIn", true);
        settings.setValue("lastCheckin", *worker.lastCheckin());
    }
    else{
        settings.setValue("hasCheckedIn", false);
    }
    if (worker.lastCheckout()) {
        settings.setValue("hasCheckedOut", true);
        settings.setValue("lastCheckout", *worker.lastCheckout());
    }
    else{
        settings.setValue("hasCheckedOut", false);
    }

    settings.setValue("checkoutOnExit", exitOption);
    settings.setValue("defaultExitDialogOption", defaultExitDialogOption);

    printWorker();
}

void Timecard::restore()
{
    QSettings settings("balenet", "Timecard");

    worker.setBalance(settings.value("balance", 0).toInt());
    worker.setWorking(settings.value("working", false).toBool());
    worker.setWorkDoneToday(settings.value("workDoneToday", 0).toInt());
    worker.setWorkdayLength(settings.value("workdayLength", DEFAULT_WORKDAY).toInt());

    bool getTime = settings.value("hasCheckedIn", false).toBool();
    if (getTime) {
         worker.setLastCheckin(settings.value("lastCheckin").toDateTime());
    }
    getTime = settings.value("hasCheckedOut", false).toBool();
    if (getTime) {
         worker.setLastCheckout(settings.value("lastCheckout").toDateTime());
    }

    exitOption = settings.value("checkoutOnExit", 0).toInt();
    exitOptionComboBox->setCurrentIndex(exitOption);
    defaultExitDialogOption = settings.value("defaultExitDialogOption",
                                             onExitCheckout).toInt();

    workdaySpinBox->setValue(worker.workdayLength() / 3600.0);
    useAlarm = settings.value("useAlarm", true).toBool();

    alarmCheckBox->setChecked(useAlarm);

    printWorker();

}

// If the user is checked in and wants to be asked what to do on exit, show the dialog,
// preselecting the option that was chosen last time. Finally save the user choices for
// the next time.
void Timecard::closeEvent(QCloseEvent* event)
{
    qDebug() << "Closing...";
    if (worker.isWorking()) {
        if (exitOption == onExitAsk) {
            ExitDialog dialog(this);
            if (defaultExitDialogOption == onExitCheckout) {
                dialog.checkoutOption->setChecked(true);
            }
            else {
                dialog.checkinOption->setChecked(true);
            }
            if (dialog.exec()) {
                int choice;
                if (dialog.checkoutOption->isChecked()) {
                    on_checkInToggle_clicked();
                    choice = onExitCheckout;
                }
                else {
                    choice = onExitStayCheckedIn;
                }
                if (dialog.checkBox->isChecked()) {
                    exitOption = choice;
                }
                defaultExitDialogOption = choice;
            }
            else {
                event->ignore();
                return;
            }
        }
        else if (exitOption == onExitCheckout) {
            on_checkInToggle_clicked();
        }
    }
    event->accept();
    save();
}

void Timecard::printWorker()
{
    qDebug() << "balance: " << worker.balance();
    qDebug() << "balanceInProgress: " << worker.balanceInProgress();
    qDebug() << "workDoneToday: " << worker.workDoneToday();
    qDebug() << "working: " << worker.isWorking();
    qDebug() << "work in progress: " << worker.workInProgress();
    qDebug() << "workday length: " << worker.workdayLength();
    if (worker.lastCheckin()) {
        qDebug() << "lastCheckin: " << worker.lastCheckin()->toString();
    }
    else {
        qDebug() << "not checked in yet";
    }
}

void Timecard::createUndoActions()
{
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    menuBar()->addAction(undoAction);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    menuBar()->addAction(redoAction);
}

void Timecard::setAlarm()
{
    if (alarm) {
        assert(worker.isWorking());

        if (!worker.isOvertime()) {
            QDateTime alarmTime = worker.lastCheckin()->addSecs(worker.workdayLength() - worker.workDoneToday());
            alarm->setTime(alarmTime);
            alarm->set();
        }
    }
}

void Timecard::removeAlarm()
{
    if (alarm) alarm->remove();
}

void Timecard::on_alarmCheckBox_toggled(bool b)
{
    useAlarm = b;

    if (worker.isWorking()) {
        if (useAlarm) {
            setAlarm();
        }
        else {
            removeAlarm();
        }
    }
}

Alarm* Timecard::createAlarm()
{
#ifdef Q_WS_HILDON
    Maemo5Alarm* a;

    QList<alarm_id_t> alarmIds;
    Maemo5Alarm::getAlarms(APP_ID, alarmIds);
    qDebug() << "Found " << alarmIds.count() << " alarms";

    assert(alarmIds.count() <=1);

    if (alarmIds.count() == 1) {
        assert(worker.isWorking() && useAlarm);
        a = new Maemo5Alarm(alarmIds.first());
        qDebug() << "existing alarm loaded, id " << alarmIds.first();
    }
    else {
        assert(!(worker.isWorking() && !worker.isOvertime())|| !useAlarm);
        a = new Maemo5Alarm(APP_ID);
        a->addDismissAction("Ok");
        a->addSnoozeAction("Remind me later");
    }
#else
    TestAlarm* a;
    a = new TestAlarm();
#endif
    a->setMessage("It's time to go home!");

    return a;
}
