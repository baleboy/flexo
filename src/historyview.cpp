#include "historyview.h"
#include "worker.h"
#include "historymodel.h"
#include "historydelegate.h"

HistoryView::HistoryView(Worker *worker, QWidget *parent) :
    QMainWindow(parent), m_worker(worker) {
    setupUi(this);

    HistoryModel *model = new HistoryModel(worker);
    listView->setModel(model);
    HistoryDelegate *delegate = new HistoryDelegate(this);
    listView->setItemDelegate(delegate);

    connect(this, SIGNAL(modeChanged(int)), model, SLOT(setMode(int)));

    QActionGroup *filterGroup = new QActionGroup(this);
    filterGroup->setExclusive(true);

    QAction *dayAction = new QAction(tr("Days"), filterGroup);
    dayAction->setCheckable(true);
    dayAction->setChecked(true);
    connect(dayAction, SIGNAL(triggered()), this, SLOT(dayModeSelected()));

    QAction *weekAction = new QAction(tr("Weeks"), filterGroup);
    weekAction->setCheckable(true);
    connect(weekAction, SIGNAL(triggered()), this, SLOT(weekModeSelected()));

    QAction *monthAction = new QAction(tr("Months"), filterGroup);
    monthAction->setCheckable(true);
    connect(monthAction, SIGNAL(triggered()), this, SLOT(monthModeSelected()));

    menuBar()->addActions(filterGroup->actions());

    this->setWindowTitle(tr("History (Days)"));
}

void HistoryView::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void HistoryView::dayModeSelected()
{
    this->setWindowTitle(tr("History (Days)"));
    emit modeChanged(HistoryModel::dayMode);
}

void HistoryView::weekModeSelected()
{
    this->setWindowTitle(tr("History (Weeks)"));
    emit modeChanged(HistoryModel::weekMode);
}

void HistoryView::monthModeSelected()
{
    this->setWindowTitle(tr("History (Months)"));
    emit modeChanged(HistoryModel::monthMode);
}

void HistoryView::showEvent ( QShowEvent * event )
{
    if (m_worker->records() == 0)
        stackedWidget->setCurrentIndex(1);
    else
        stackedWidget->setCurrentIndex(0);
    return QWidget::showEvent(event);
}
