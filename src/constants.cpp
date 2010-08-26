#include <QDir>
#include <QDateTime>

#include "constants.h"

const int DEFAULT_WORKDAY = 8.0*3600.0;
const int TIMER_VALUE_IN_MSEC = 60000;

const QString ORG_ID = "com.balenet";
const QString APP_NAME = "flexo";
const QString APP_ID = ORG_ID + "." + APP_NAME;
const QString DBUS_PATH = "/com/balenet/flexo";
const QString SAVEFILE = QDir::home().filePath(".flexo.data");

// if compiled for test, replace the actual clock with a global variable
// that can be manipulated by the test program
#ifdef WORKER_TEST
QDateTime clock_;
#endif
