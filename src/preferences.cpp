#include <QSettings>

#include "preferences.h"
#include "constants.h"

Preferences::Preferences() :
    m_useAlarm(true),
    m_exitOption(onExitAsk),
    m_defaultExitDialogOption(onExitCheckout),
    m_unlockScreen(true),
    m_balanceUnit(0)
{
}

void Preferences::load()
{
    QSettings settings(ORG_ID, APP_NAME);

    m_exitOption = settings.value("checkoutOnExit", onExitAsk).toInt();
    m_defaultExitDialogOption = settings.value("defaultExitDialogOption",
                                               onExitCheckout).toInt();
    m_useAlarm = settings.value("useAlarm", true).toBool();
    m_unlockScreen = settings.value("unlockScreen", true).toBool();
    m_balanceUnit = settings.value("balanceUnit", 0).toInt();
}

void Preferences::save()
{
    QSettings settings(ORG_ID, APP_NAME);

    settings.setValue("useAlarm", m_useAlarm);
    settings.setValue("checkoutOnExit", m_exitOption);
    settings.setValue("defaultExitDialogOption", m_defaultExitDialogOption);
    settings.setValue("unlockScreen", m_unlockScreen);
    settings.setValue("balanceUnit", m_balanceUnit);
}
