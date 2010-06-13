#include <QUndoStack>
#include <QtDebug>
#include <QMaemo5ListPickSelector>
#include <QStandardItemModel>
#include <QStandardItem>

#include "balancewindow.h"
#include "commands.h"
#include "constants.h"
#include "worker.h"
#include "preferences.h"

BalanceWindow::BalanceWindow(QUndoStack *u, Worker *w,
                             Preferences *p, QWidget *parent) :
QWidget(parent),
m_undoStack(u),
m_worker(w),
m_preferences(p)

{
    setupUi(this);

    // Create the picker button and related selector
    QStandardItemModel *model = new QStandardItemModel(2, 1);
    model->setItem(0, 0, new QStandardItem("Hours"));
    model->setItem(1, 0, new QStandardItem("Days"));

    QMaemo5ListPickSelector *sel = new QMaemo5ListPickSelector;
    sel->setModel(model);
    sel->setCurrentIndex(m_preferences->balanceUnit());
    balanceUnitButton->setPickSelector(sel);
    balanceUnitButton->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    connect(sel, SIGNAL(selected(QString)), this, SLOT(showBalance()));
}

void BalanceWindow::on_balanceEdit_editingFinished()
{
    QMaemo5ListPickSelector *sel =
            qobject_cast<QMaemo5ListPickSelector *>(balanceUnitButton->pickSelector());
    int newValue;
    if (sel->currentIndex() == 0)
        newValue = int(balanceEdit->value() * 3600.0);
    else
        newValue = int(balanceEdit->value()) * m_worker->workdayLength();

    if (newValue != m_worker->balance())
        m_undoStack->push(new EditBalanceCommand(this, newValue, m_worker));
}

void BalanceWindow::showBalance()
{
    QMaemo5ListPickSelector *sel =
            qobject_cast<QMaemo5ListPickSelector *>(balanceUnitButton->pickSelector());
    double value;
    if (sel->currentIndex() == 0)
        value = double(m_worker->balanceInProgress()) / 3600.0;
    else
        value = double(m_worker->balanceInProgress()) /
                double(m_worker->workdayLength());
    balanceEdit->setValue(value);
    balanceEdit->setDisabled(m_worker->isWorking());

    if (sel->currentIndex() != m_preferences->balanceUnit())
        m_preferences->setBalanceUnit(sel->currentIndex());

    qDebug() << "balance: " << value;
}

void BalanceWindow::showEvent(QShowEvent *event)
{
    showBalance();
}
