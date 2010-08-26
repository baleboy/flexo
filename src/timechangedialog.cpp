#include "timechangedialog.h"

TimeChangeDialog::TimeChangeDialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
}

void TimeChangeDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
