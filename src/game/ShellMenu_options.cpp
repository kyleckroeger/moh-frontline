// A fragment of ShellMenu.cpp (0x800df1cc): playing a shell sound (when the
// flag at +6408 is set, an event record copied from an initializer in the
// file's .sdata2 pool, a pool reference, with the sound filled in), counting
// a completed objective, marking the new-game sign once, the crosshair
// setting, and the controller options (four signed byte settings in a record at +532): applying two of
// them to the input manager's flags, passing all four to the interface studio
// and setting them from it (which also sets the flag at +536). The record's
// inline accessors check their this pointer (-1 for a missing record), as the
// target's null checks of the record's address show. CShellMenu, CInputMgr
// and the functions are named by the mangled symbols; the members, the
// record view, the event record and the result types are inferred. The rest
// of the file is not part of this unit.
extern "C" void* memcpy(void*, const void*, unsigned long);

class CInputMgr {
public:
    unsigned char unknown0000[9285];
    bool m_flag9285;
    unsigned char unknown2446[4];
    bool m_flag9290;
};

extern CInputMgr g_inputMgr;

struct ControllerOptionsView {
    signed char m_settings[4];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
    void Set(int index, unsigned char value) {
        if (this)
            m_settings[index] = value;
    }
};

void AEMS_SendEvent(void*);

struct ShellSoundEvent {
    int event;
    int sound;
};

class CShellMenu {
public:
    void PlayShellSound(int);
    void CompletedAnotherObjective();
    void ChangeNewGameSignStatus();
    unsigned char GetCrosshairStatus();
    void InitControllerFromOptions();
    int PassControllerOptionsSettingToIStudio(int*);
    void SetupControllerOptionsSettings(int*);

    unsigned char unknown0000[14];
    unsigned char m_objectivesCompleted;
    unsigned char unknown000f[517];
    ControllerOptionsView m_options;
    unsigned char m_flag536_0 : 1;
    unsigned char m_flag536_1 : 1;
    unsigned char m_flag536_2 : 1;
    unsigned char m_flag536_3 : 1;
    unsigned char unknown536 : 4;
    unsigned char unknown0219[59];
    bool m_newGameSign;
    unsigned char unknown0255[5811];
    bool m_soundEnabled;
};

void CShellMenu::PlayShellSound(int sound) {
    if (m_soundEnabled) {
        ShellSoundEvent event = {11, 0};
        event.sound = sound;
        AEMS_SendEvent(&event);
    }
}

void CShellMenu::CompletedAnotherObjective() {
    m_objectivesCompleted++;
}

void CShellMenu::ChangeNewGameSignStatus() {
    if (!m_newGameSign)
        m_newGameSign = true;
}

unsigned char CShellMenu::GetCrosshairStatus() {
    return m_options.m_settings[1];
}

void CShellMenu::InitControllerFromOptions() {
    bool first = false;

    if (m_options.Get(0) == 1)
        first = true;
    g_inputMgr.m_flag9290 = first;
    bool second = false;
    if (m_options.Get(2) == 1)
        second = true;
    g_inputMgr.m_flag9285 = second;
}

int CShellMenu::PassControllerOptionsSettingToIStudio(int* out) {
    int settings[4];

    settings[0] = m_options.Get(0);
    settings[1] = m_options.Get(1);
    settings[2] = m_options.Get(2);
    settings[3] = m_options.Get(3);
    memcpy(out, settings, sizeof(settings));
    return 4;
}

void CShellMenu::SetupControllerOptionsSettings(int* settings) {
    m_options.Set(0, settings[0]);
    m_options.Set(1, settings[1]);
    m_options.Set(2, settings[2]);
    m_options.Set(3, settings[3]);
    m_flag536_3 = 1;
}
