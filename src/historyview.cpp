#include "historyview.h"
#include "worker.h"
#include "historymodel.h"
#include "historydelegate.h"

HistoryView::HistoryView(Worker *worker, QWidget *parent) :
    QMainWindow(parent) {
    setupUi(this);

    HistoryModel *model = new HistoryModel(worker);
    listView->setModel(model);
    HistoryDelegate *delegate = new HistoryDelegate(this);
    listView->setItemDelegate(delegate);

    connect(this, SIGNAL(modeChanged(int)), model, SLOT(setMode(int)));

    QActionGroup *filterGroup = new QActionGroup(this);
    filterGroup->setExclusive(true);

    QAction *dayAction = new QAction(tr("Day"), filterGroup);
    dayAction->setCheckable(true);
    dayAction->setChecked(true);
    connect(dayAction, SIGNAL(triggered()), this, SLOT(dayModeSelected()));

    QAction *weekAction = new QAction(tr("Week"), filterGroup);
    weekAction->setCheckable(true);
    connect(weekAction, SIGNAL(triggered()), this, SLOT(weekModeSelected()));

    QAction *monthAction = new QAction(tr("Month"), filterGroup);
    monthAction->setCheckable(true);
    connect(monthAction, SIGNAL(triggered()), this, SLOT(monthModeSelected()));

    menuBar()->addActions(filterGroup->actions());
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
    emit modeChanged(HistoryModel::dayMode);
}

void HistoryView::weekModeSelected()
{
    emit modeChanged(HistoryModel::weekMode);
}

void HistoryView::monthModeSelected()
{
    emit modeChanged(HistoryModel::monthMode);
}
