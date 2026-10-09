// A fragment of input.cpp; the file name is this project's. CInputManager, the functions at the start of the file: per-player key
// value tables, rumble feedback and keymaps. CInputManager, CDeviceManager and
// KEYMAP are named by the mangled symbols; the base-class relationship, the
// 2316-byte device record and the key and shell views are inferred from
// offsets and are not original names. The unit is compiled with
// -inline deferred,auto: GetValueAnyPlayer inlines GetValue, which follows it
// in the image, so the functions are listed in reverse image order.
struct KEYMAP;

struct InputKeyView {
    int field00;
    int field04;
    int value;
};

struct InputKeyTableView {
    InputKeyView keys[189];
};

struct InputDeviceView {
    unsigned char field0000[48];
    InputKeyTableView table;
};

struct ShellPlayerView {
    unsigned char field00[2];
    bool rumble;
    unsigned char field03[25];
};

struct ShellView {
    unsigned char field0000[5216];
    ShellPlayerView players[4];
};

extern ShellView g_Shell;
extern bool g_bInMultiplayerMode;

extern "C" void PADControlMotor(int, unsigned long);

class CDeviceManager {
public:
    int SetActuatorState(int, long, long, float, float);
    bool IsReady(int);
    void Shutdown();

    InputDeviceView m_devices[4];
};

class CInputManager : public CDeviceManager {
public:
    void Reset();
    int VerifyAnalog();
    bool IsReady();
    void SetFeedback(unsigned long, long, long, float, float);
    void ResetActuatorState(long);
    bool GetValueAnyPlayer(unsigned long);
    int GetValue(unsigned long, unsigned long);
    void SetKeymap(unsigned long, KEYMAP*);
    void Shutdown();

    InputKeyTableView* GetKeyTable(unsigned long player) { return &m_devices[player].table; }

    KEYMAP* m_keymap;
    KEYMAP* m_playerKeymaps[4];
    unsigned char field2444;
    bool m_feedback;
    bool m_feedbackOff[4];
};

void CInputManager::Shutdown() {
    CDeviceManager::Shutdown();
}

void CInputManager::SetKeymap(unsigned long player, KEYMAP* keymap) {
    if (g_bInMultiplayerMode)
        m_playerKeymaps[player] = keymap;
    else
        m_keymap = keymap;
}

int CInputManager::GetValue(unsigned long player, unsigned long key) {
    InputKeyTableView* table = GetKeyTable(player);
    return table ? table->keys[key].value : 0;
}

bool CInputManager::GetValueAnyPlayer(unsigned long key) {
    bool down = GetValue(0, key) != 0;
    if (!down) down = GetValue(1, key) != 0;
    if (!down) down = GetValue(2, key) != 0;
    if (!down) down = GetValue(3, key) != 0;
    return down;
}

void CInputManager::ResetActuatorState(long state) {
    for (int i = 0; i < 4; i++) {
        SetActuatorState(i, state, 0, 0.0f, 1.0f);
        if (state == 0)
            PADControlMotor(i, 2);
    }
}

void CInputManager::SetFeedback(unsigned long player, long a, long b, float c, float d) {
    bool enabled = false;

    if (g_bInMultiplayerMode) {
        if (g_Shell.players[player].rumble)
            enabled = true;
    } else {
        enabled = m_feedback;
    }
    if (enabled && !m_feedbackOff[player])
        SetActuatorState(player, a, b, c, d);
}

bool CInputManager::IsReady() {
    for (int i = 0; i < 4; i++) {
        if (CDeviceManager::IsReady(i))
            return true;
    }
    return false;
}

int CInputManager::VerifyAnalog() {
    return 1;
}

void CInputManager::Reset() {
    InputDeviceView* device = m_devices;
    InputKeyTableView* table;
    int i;

    for (i = 0; i < 4; i++, device++) {
        table = &device->table;
        if (table) {
            for (int j = 0; j < 189; j++) {
                table->keys[j].value = 0;
                table->keys[j].field04 = 0;
            }
        }
    }
}
