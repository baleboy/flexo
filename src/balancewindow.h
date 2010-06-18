#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include "ui_balancewindow.h"

class QUndoStack;
class Worker;
class Preferences;

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

private:

    QUndoStack *m_undoStack;
    Worker *m_worker;
    Preferences *m_preferences;
    QString m_currentText;
};

#endif // BALANCEWINDOW_H
