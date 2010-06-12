#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include "ui_preferenceswindow.h"

class Worker;
class Preferences;

class PreferencesWindow : public QWidget, private Ui::PreferencesWindow
{
    Q_OBJECT

public:

    explicit PreferencesWindow(Preferences*, Worker*, QWidget *parent = 0);

signals:
    void alarmToggled(bool);
    void workdayUpdated();

public slots:
    void on_workdaySpinBox_editingFinished();
    void on_alarmCheckBox_toggled(bool b);
    void on_unlockCheckBox_toggled(bool b);
    void changeExitOption();

private:
    Worker* m_worker;
    Preferences* m_preferences;
};

#endif // PREFERENCESWINDOW_H
