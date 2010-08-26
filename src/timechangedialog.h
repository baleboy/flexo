#ifndef TIMECHANGEDIALOG_H
#define TIMECHANGEDIALOG_H

#include <QMaemo5AbstractPickSelector>

#include "ui_timechangedialog.h"

class TimeChangeDialog : public QDialog, public Ui::TimeChangeDialog
{
    Q_OBJECT

public:
    explicit TimeChangeDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // TIMECHANGEDIALOG_H
