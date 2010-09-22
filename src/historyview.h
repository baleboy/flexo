#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include "ui_historyview.h"

class Worker;

class HistoryView : public QMainWindow, private Ui::HistoryView
{
    Q_OBJECT

public:
    explicit HistoryView(Worker *worker, QWidget *parent = 0);

public slots:

    void dayModeSelected();
    void weekModeSelected();
    void monthModeSelected();

signals:
    void modeChanged(int mode);

protected:
    void changeEvent(QEvent *e);
    void showEvent (QShowEvent *e);

private:

    Worker *m_worker;
};

#endif // HISTORYVIEW_H
