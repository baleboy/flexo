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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commands.h"
#include "exitdialog.h"
#include "testalarm.h"
#include "maemo5alarm.h"

const int MainWindow::DEFAULT_WORKDAY = 7.5*3600;
const int MainWindow::TIMER_VALUE_IN_MSEC = 60000;

const QString MainWindow::ORG_ID = "balenet.com";
const QString MainWindow::APP_NAME = "flexo";
const QString MainWindow::APP_ID = ORG_ID + "." + APP_NAME;
const QString MainWindow::DBUS_PATH = "/com/balenet/flexo";
const QString MainWindow::SAVEFILE = "data.flexo";

enum { onExitAsk = 0, onExitCheckout = 1, onExitStayCheckedIn = 2 };

MainWindow::MainWindow(QWidget* parent)
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

void MainWindow::updateView()
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
        displayBalance();
}

void MainWindow::setBalance(int balance)
{
    worker.setBalance(balance);
    double b;
    if (showBalanceInHours) {
        b = worker.balance() / 3600.0;
    }
    else {
        b = worker.balance() / (double)worker.workdayLength();
    }
    balanceEdit->setValue(b);
}

void MainWindow::on_balanceEdit_editingFinished()
{
    qDebug() << "Editing finished, new value: " << balanceEdit->value();

    double b = balanceEdit->text().toDouble();
    int balance;

    if (showBalanceInHours) {
        balance = b * 3600;
    }
    else {
        balance = b * worker.workdayLength();
    }
    if (balance != worker.balance()) {
        undoStack->push(new EditBalanceCommand(this, balance, worker.balance()));
    }

    qDebug() << "Balance: " << worker.balance();
}

void MainWindow::on_balanceUnitSelector_currentIndexChanged(int index)
{
        qDebug() << "Index changed: " << index;
	showBalanceInHours = (index == 0);
	displayBalance();
}

void MainWindow::on_actionClock_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(clockPage);
        setWindowTitle(actionClock->text());
    }
}

void MainWindow::on_actionBalance_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(balancePage);
        setWindowTitle(actionBalance->text());
    }
}

void MainWindow::on_actionSettings_toggled(bool checked)
{
    if (checked) {
        mainView->setCurrentWidget(settingsPage);
        setWindowTitle(actionSettings->text());
    }
}

void MainWindow::displayTimeAtWork()
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

void MainWindow::displayBalance()
{	
        double b = worker.balanceInProgress();
	if (showBalanceInHours) {
                b = b / 3600.0;
	}
	else {
                b = b / (double)worker.workdayLength();
	}
        balanceEdit->setValue(b);
}

void MainWindow::reset()
{
    qDebug() << "Reset";

    undoStack->push(new ClearAllCommand(this, worker, timer));

}

void MainWindow::on_exitOptionComboBox_currentIndexChanged(int option)
{
    exitOption = option;
}

void MainWindow::on_workdaySpinBox_editingFinished()
{
    worker.setWorkdayLength(workdaySpinBox->value() * 3600);
    updateView();
}

void MainWindow::save()
{
    QSettings settings(ORG_ID, APP_NAME);

    settings.setValue("useAlarm", useAlarm);
    settings.setValue("checkoutOnExit", exitOption);
    settings.setValue("defaultExitDialogOption", defaultExitDialogOption);

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

    QSettings settings(ORG_ID, APP_NAME);

    exitOption = settings.value("checkoutOnExit", 0).toInt();
    exitOptionComboBox->setCurrentIndex(exitOption);
    defaultExitDialogOption = settings.value("defaultExitDialogOption",
                                             onExitCheckout).toInt();

    useAlarm = settings.value("useAlarm", true).toBool();
    alarmCheckBox->setChecked(useAlarm);

    workdaySpinBox->setValue(worker.workdayLength() / 3600.0);
}

// If the user is checked in and wants to be asked what to do on exit, show the dialog,
// preselecting the option that was chosen last time. Finally save the user choices for
// the next time.
void MainWindow::closeEvent(QCloseEvent* event)
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
    if (alarm) {
        assert(worker.isWorking());

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

void MainWindow::on_alarmCheckBox_toggled(bool b)
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

void MainWindow::Activate()
{
    activateWindow();
}

Alarm* MainWindow::createAlarm()
{
#ifdef Q_WS_MAEMO_5
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
