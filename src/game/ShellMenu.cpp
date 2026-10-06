// CShellMenu accessors: the current stage, mission, difficulty and controller
// setting and the stage and mission setters. CShellMenu is named by the
// mangled symbols; the members are inferred from the accessors' offsets and
// named after them. The difficulty setter that follows and the rest of the
// file are not part of this unit.
class CShellMenu {
public:
    unsigned int Get_currentStage();
    unsigned int Get_currentMission();
    unsigned char Get_currentDifficulty();
    unsigned char Get_currentControllerSetting();
    void Set_currentStage(unsigned int);
    void Set_currentMission(unsigned int);

    unsigned char m_currentControllerSetting;
    unsigned char m_currentDifficulty;
    unsigned char unknown02[2];
    unsigned int m_currentMission;
    unsigned int m_currentStage;
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
