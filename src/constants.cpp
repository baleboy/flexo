#include <QDir>
#include "constants.h"

const int DEFAULT_WORKDAY = 7.5*3600;
const int TIMER_VALUE_IN_MSEC = 60000;

const QString ORG_ID = "balenet.com";
const QString APP_NAME = "flexo";
const QString APP_ID = ORG_ID + "." + APP_NAME;
const QString DBUS_PATH = "/com/balenet/flexo";
const QString SAVEFILE = QDir::home().filePath(".flexo");
