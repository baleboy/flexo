#include "preferenceswindow.h"
#include "worker.h"
#include "preferences.h"

PreferencesWindow::PreferencesWindow(Preferences* p, Worker* w, QWidget *parent) :
    QWidget(parent), m_worker(w), m_preferences(p)
{
    setupUi(this);
    workdaySpinBox->setValue(double(m_worker->workdayLength()) / 3600.0);
    alarmCheckBox->setChecked(m_preferences->useAlarm());
    exitOptionComboBox->setCurrentIndex(m_preferences->exitOption());
    unlockCheckBox->setChecked(m_preferences->unlockScreen());
}

void PreferencesWindow::on_workdaySpinBox_editingFinished()
{
    m_worker->setWorkdayLength(int(workdaySpinBox->value()) * 3600);
}

void PreferencesWindow::on_exitOptionComboBox_currentIndexChanged(int option)
{
    m_preferences->setExitOption(option);
}

void PreferencesWindow::on_alarmCheckBox_toggled(bool b)
{
    m_preferences->setUseAlarm(b);
    emit alarmToggled(b);
}

void PreferencesWindow::on_unlockCheckBox_toggled(bool b)
{
    m_preferences->setUnlockScreen(b);
}
