// A fragment of bsbifunc.cpp (0x8002b064): script built-ins that initialise
// debugging from a trigger (no effect), load a one-shot sound bank (for the
// multiplayer mission and level stored in the shell, otherwise the shell's
// current mission and stage) and remember the bank, mission and level, and
// show or hide the first player's letterbox. Each reads its argument below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. The functions, classes and globals are named by the
// mangled symbols; the shell, scene and player views are inferred (members at
// their offsets, names not original), as is the built-in record view.
class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();

    unsigned char unknown0000[5148];
    int m_mpMission;
    int m_mpLevel;
};

class CPlayerObject {
public:
    unsigned char unknown0000[4720];
    bool m_letterbox;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CShellMenu g_Shell;
extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern int g_CurrentMission;
extern int g_CurrentLevel;
extern int g_CurrentSoundBank;

void LoadOneShotSound(int, int, int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_DebugInitFromTrigger(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_LoadOneShotSoundBank(int** stack, void*) {
    int bank = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int mission;
    int level;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mpMission;
        level = g_Shell.m_mpLevel;
    } else {
        mission = g_Shell.Get_currentMission();
        level = g_Shell.Get_currentStage();
    }
    LoadOneShotSound(bank, mission, level);
    g_CurrentMission = mission;
    g_CurrentLevel = level;
    g_CurrentSoundBank = bank;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DisplayLetterbox(int** stack, void*) {
    bool show = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    g_scene.GetPlayer(0)->m_letterbox = show;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
