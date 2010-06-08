#ifndef TIMEDIALOG_H
#define TIMEDIALOG_H

#include "ui_timedialog.h"

class TimeDialog : public QDialog, public Ui::TimeDialog
{
    Q_OBJECT

public:
    explicit TimeDialog(QWidget *parent = 0);
};

#endif // TIMEDIALOG_H
