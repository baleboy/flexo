#include <QMaemo5ListPickSelector>
#include <QMaemo5ListPickSelector>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QtDebug>

#include "preferenceswindow.h"
#include "worker.h"
#include "preferences.h"

PreferencesWindow::PreferencesWindow(Preferences* p, Worker* w, QWidget *parent) :
    QWidget(parent), m_worker(w), m_preferences(p)
{
    setupUi(this);
    workdaySpinBox->setValue(double(m_worker->workdayLength()) / 3600.0);
    alarmCheckBox->setChecked(m_preferences->useAlarm());
    unlockCheckBox->setChecked(m_preferences->unlockScreen());

    // Create the picker button and related selector
    QStandardItemModel *model = new QStandardItemModel(3, 1);
    model->setItem(Preferences::onExitAsk, 0, new QStandardItem("Ask what to do"));
    model->setItem(Preferences::onExitCheckout, 0, new QStandardItem("Check out"));
    model->setItem(Preferences::onExitStayCheckedIn, 0, new QStandardItem("Stay checked in"));

    QMaemo5ListPickSelector *sel = new QMaemo5ListPickSelector;
    sel->setModel(model);
    sel->setCurrentIndex(m_preferences->exitOption());
    exitOptionButton->setPickSelector(sel);
    exitOptionButton->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    connect(sel, SIGNAL(selected(const QString&)), this, SLOT(changeExitOption()));
}

void PreferencesWindow::changeExitOption()
{
    QMaemo5ListPickSelector *sel =
            qobject_cast<QMaemo5ListPickSelector *>(exitOptionButton->pickSelector());
    m_preferences->setExitOption(sel->currentIndex());
}

void PreferencesWindow::on_workdaySpinBox_editingFinished()
{
    m_worker->setWorkdayLength(int(workdaySpinBox->value() * 3600));
    emit workdayUpdated();
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
