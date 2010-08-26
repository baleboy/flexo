#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QDateTime>

extern const int DEFAULT_WORKDAY;
extern const int TIMER_VALUE_IN_MSEC;

extern const QString ORG_ID;
extern const QString APP_NAME;
extern const QString APP_ID;
extern const QString DBUS_PATH;
extern const QString SAVEFILE;
extern const QString LEGACY_SAVEFILE;

#ifdef WORKER_TEST
extern QDateTime clock_;
#define CURRENT_TIME clock_
#else
#define CURRENT_TIME QDateTime::currentDateTime()
#endif

#endif // CONSTANTS_H
