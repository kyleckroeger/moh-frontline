// CShellMenu accessors: the current stage, mission, difficulty and controller
// setting, the stage and mission setters, the difficulty setter (unless
// locked: a flag at +533 records whether difficulty 1 is chosen, and the lock
// is set once the flag at +6568 is) and the controller-setting setter.
// CShellMenu is named by the mangled symbols; the members are inferred from
// the accessors' offsets and named after them. The record at +532 is an
// inferred view: the target null-checks its address before each store, which
// an inline setter with a this check reproduces. The rest of the file is not
// part of this unit.
struct ShellRecord532View {
    unsigned char unknown0;
    bool m_flag;
    unsigned char unknown2[2];

    void SetFlag(bool flag) {
        if (this)
            m_flag = flag;
    }
};

class CShellMenu {
public:
    unsigned int Get_currentStage();
    unsigned int Get_currentMission();
    unsigned char Get_currentDifficulty();
    unsigned char Get_currentControllerSetting();
    void Set_currentStage(unsigned int);
    void Set_currentMission(unsigned int);
    void Set_currentDifficulty(unsigned char);
    void Set_currentControllerSetting(unsigned char);

    unsigned char m_currentControllerSetting;
    unsigned char m_currentDifficulty;
    unsigned char unknown002[2];
    unsigned int m_currentMission;
    unsigned int m_currentStage;
    unsigned char unknown00c[520];
    ShellRecord532View m_record532;
    unsigned char m_flag536_0 : 1;
    unsigned char m_flag536_1 : 1;
    unsigned char m_flag536_2 : 1;
    unsigned char m_difficultyLocked : 1;
    unsigned char unknown536 : 4;
    unsigned char unknown219[6031];
    bool m_flag6568;
};

unsigned int CShellMenu::Get_currentStage() {
    return m_currentStage;
}

unsigned int CShellMenu::Get_currentMission() {
    return m_currentMission;
}

unsigned char CShellMenu::Get_currentDifficulty() {
    return m_currentDifficulty;
}

unsigned char CShellMenu::Get_currentControllerSetting() {
    return m_currentControllerSetting;
}

void CShellMenu::Set_currentStage(unsigned int stage) {
    m_currentStage = stage;
}

void CShellMenu::Set_currentMission(unsigned int mission) {
    m_currentMission = mission;
}

void CShellMenu::Set_currentDifficulty(unsigned char difficulty) {
    m_currentDifficulty = difficulty;
    if (m_difficultyLocked)
        return;
    if (m_currentDifficulty == 1)
        m_record532.SetFlag(true);
    else
        m_record532.SetFlag(false);
    if (m_flag6568)
        m_difficultyLocked = 1;
}

void CShellMenu::Set_currentControllerSetting(unsigned char setting) {
    m_currentControllerSetting = setting;
}
