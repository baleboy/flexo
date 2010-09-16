#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include "ui_balancewindow.h"

class QUndoStack;
class Worker;
class Preferences;
class HistoryView;

class BalanceWindow : public QWidget, private Ui::BalanceWindow
{
    Q_OBJECT

public:
    explicit BalanceWindow(QUndoStack*, Worker*, Preferences*, QWidget *parent = 0);

public slots:

    void showBalance();

protected:

    void showEvent(QShowEvent *);

private slots:
    void on_balanceEdit_editingFinished();
    void on_historyButton_clicked();

private:

    QUndoStack *m_undoStack;
    Worker *m_worker;
    Preferences *m_preferences;
    QString m_currentText;
    HistoryView* m_historyView;
};

#endif // BALANCEWINDOW_H
