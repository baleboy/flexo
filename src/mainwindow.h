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
class BalanceWindow;
class ExitDialog;
class Preferences;
class PreferencesWindow;

/*!
 * Implementation of the application logic for the main window.
 */
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void save();
    void restore();

public slots:

    void updateView();
    void Activate();
    void setAlarm();
    void removeAlarm();

private slots:

    void on_checkInToggle_clicked();
    void on_balanceButton_clicked();
    void on_settingsButton_clicked();

    void updateTime();

    void toggleAlarm(bool);

    void reset();

    void onTimer();

protected:
    void closeEvent(QCloseEvent *event);

private:

    QTimer *timer;

    QString textBuffer;

    Worker worker;

    void displayTimeAtWork();
    void displayBalance();
    void createUndoActions();
    void showWarning(const QString&);
    QString timeText(const QDateTime&);

    Alarm* createAlarm();

    QUndoStack *undoStack;
    QAction* undoAction;
    QAction* redoAction;
    QMenu *editMenu;
    QActionGroup *toolbarGroup;

    Alarm* alarm;

    BalanceWindow* m_balanceWindow;
    PreferencesWindow* m_preferencesWindow;

    Preferences* m_preferences;
}; 
