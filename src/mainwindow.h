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

#include <QMainWindow>
#include <QTime>

#include "ui_mainwindow.h"
#include "worker.h"

class QTimer;
class QUndoStack;
class QActionGroup;
class Alarm;

class ExitDialog;

/*!
 * Implementation of the application logic for the main window.
 */
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    void updateView();
    void setAlarm();
    void removeAlarm();
    void setBalance(int);

    static const QString ORG_ID;
    static const QString APP_NAME;
    static const QString APP_ID;
    static const QString DBUS_PATH;

public slots:

    void Activate();

private slots:

    void on_checkInToggle_clicked();
    void on_balanceEdit_editingFinished();
    void on_balanceUnitSelector_currentIndexChanged(int);
    void on_exitOptionComboBox_currentIndexChanged(int);
    void on_workdaySpinBox_editingFinished();
    void on_alarmCheckBox_toggled(bool);

    void on_actionClock_toggled(bool);
    void on_actionBalance_toggled(bool);
    void on_actionSettings_toggled(bool);

    void reset();

    void onTimer();

protected:
    void closeEvent(QCloseEvent *event);

private:

    QTimer *timer;

    QString textBuffer;

    Worker worker;

    bool showBalanceInHours;
    int exitOption;
    int defaultExitDialogOption;

    void displayTimeAtWork();
    void displayBalance();
    void save();
    void restore();
    void printWorker();
    void createUndoActions();
    Alarm* createAlarm();

    QUndoStack *undoStack;
    QAction* undoAction;
    QAction* redoAction;
    QMenu *editMenu;
    QActionGroup *toolbarGroup;

    Alarm* alarm;
    bool useAlarm;

    static const int DEFAULT_WORKDAY;
    static const int TIMER_VALUE_IN_MSEC;
}; 
