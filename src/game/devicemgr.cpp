// CDeviceManager, the functions from GetDeviceState to the constructor: the
// manager holds four controller devices, reports a device's state while its
// pad reads without error, updates the pads and each device, and initialises
// each device's port, cleared state and copy of the master event table.
// CDeviceManager and CGCDevice are named by the mangled symbols; the device
// layout, the pad data view and the table's type are inferred from offsets.
// UpdateActuators and SetActuatorState before this run (the latter uses the
// file's constants and a jump table) are not part of the unit.
extern "C" {
void* memset(void*, int, unsigned long);
void* memcpy(void*, const void*, unsigned long);
}

struct PADDATAVIEW {
    unsigned char unknown00[10];
    signed char err;
};

extern "C" {
PADDATAVIEW* PAD_getdataptr(int);
void PAD_update();
}

extern unsigned char masterEventTable[];

class CGCDevice {
public:
    void Update();

    bool m_ready;
    unsigned char unknown01[3];
    int m_port;
    unsigned char m_state[2];
    unsigned char m_cleared[12];
    unsigned char unknown16[26];
    unsigned char m_events[2268];
};

class CDeviceManager {
public:
    CDeviceManager();
    ~CDeviceManager();
    void* GetDeviceState(unsigned long);
    bool IsReady(int);
    void Update();
    void Shutdown();
    void Init();

    CGCDevice m_devices[4];
};

void* CDeviceManager::GetDeviceState(unsigned long index) {
    if (PAD_getdataptr(m_devices[index].m_port)->err == 0)
        return m_devices[index].m_state;
    return 0;
}

bool CDeviceManager::IsReady(int index) {
    return m_devices[index].m_ready;
}

void CDeviceManager::Update() {
    PAD_update();
    for (int i = 0; i < 4; i++)
        m_devices[i].Update();
}

void CDeviceManager::Shutdown() {
}

void CDeviceManager::Init() {
    for (int i = 0; i < 4; i++) {
        m_devices[i].m_port = i;
        memset(m_devices[i].m_cleared, 0, 12);
        m_devices[i].m_ready = false;
        memcpy(m_devices[i].m_events, masterEventTable, 2268);
    }
}

CDeviceManager::~CDeviceManager() {
}

CDeviceManager::CDeviceManager() {
}
