#include <QUndoStack>
#include <QtDebug>

#include "balancewindow.h"
#include "commands.h"
#include "constants.h"
#include "worker.h"

BalanceWindow::BalanceWindow(QUndoStack *u, Worker* w, QWidget *parent) :
    QWidget(parent), m_undoStack(u), m_worker(w)
{
    setupUi(this);
    balanceUnitSelector->setCurrentIndex(0);
    connect(balanceUnitSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showBalance()));
}

void BalanceWindow::on_balanceEdit_editingFinished()
{
    int newValue;
    if (balanceUnitSelector->currentIndex() == 0)
        newValue = int(balanceEdit->value() * 3600.0);
    else
        newValue = int(balanceEdit->value()) * m_worker->workdayLength();

    if (newValue != m_worker->balance())
        m_undoStack->push(new EditBalanceCommand(this, newValue, m_worker));

}

void BalanceWindow::showBalance()
{
    double value;
    if (balanceUnitSelector->currentIndex() == 0)
        value = double(m_worker->balanceInProgress()) / 3600.0;
    else
        value = double(m_worker->balanceInProgress()) /
                double(m_worker->workdayLength());
    balanceEdit->setValue(value);
    balanceEdit->setDisabled(m_worker->isWorking());

    qDebug() << "balance: " << value;
}

void BalanceWindow::showEvent(QShowEvent *event)
{
    showBalance();
}
