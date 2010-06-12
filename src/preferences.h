#ifndef PREFERENCES_H
#define PREFERENCES_H

class Preferences
{
public:

    enum { onExitAsk = 0, onExitCheckout = 1, onExitStayCheckedIn = 2 };

    Preferences();

    void save();
    void load();

    bool useAlarm() { return m_useAlarm; }
    int exitOption() { return m_exitOption; }
    int defaultExitDialogOption() { return m_defaultExitDialogOption; }
    bool unlockScreen() { return m_unlockScreen; }

    void setUseAlarm(bool use) { m_useAlarm = use; }
    void setExitOption(int opt) { m_exitOption = opt; }
    void setDefaultExitDialogOption(int opt) { m_defaultExitDialogOption = opt; }
    void setUnlockScreen(bool unlock) { m_unlockScreen = unlock; }

private:
    bool m_useAlarm;
    int m_exitOption;
    int m_defaultExitDialogOption;
    int m_workdayLength;
    bool m_unlockScreen;
};

#endif // PREFERENCES_H
