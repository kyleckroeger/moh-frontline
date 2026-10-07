// A fragment of ShellMenu.cpp (0x800e1e58): whether the current stage is the
// last level of the current mission (a local table of the last level per
// mission) and the empty PlayRewardMovie. CShellMenu and the functions are
// named by the mangled symbols; the members (as in ShellMenu.cpp) and the
// result type are inferred. PlayGalleryMovie after this uses the file's
// string pool and is not part of this unit, nor is the rest of the file.
class CShellMenu {
public:
    unsigned char CheckIfThisIsTheLastLevelForTheMission();
    void PlayRewardMovie(int);

    unsigned char unknown00[4];
    unsigned int m_currentMission;
    unsigned int m_currentStage;
};

unsigned char CShellMenu::CheckIfThisIsTheLastLevelForTheMission() {
    unsigned char lastLevel[7];

    lastLevel[0] = 0;
    lastLevel[1] = 4;
    lastLevel[2] = 3;
    lastLevel[3] = 3;
    lastLevel[4] = 3;
    lastLevel[5] = 4;
    lastLevel[6] = 2;
    return m_currentStage == lastLevel[m_currentMission];
}

void CShellMenu::PlayRewardMovie(int) {
}
