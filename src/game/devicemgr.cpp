// CDeviceManager, the functions at the start of the file: the manager holds
// four controller devices; it updates their actuators, sets an actuator's
// state (a level chosen by the actuator type, and a duration), reports a
// device's state while its pad reads without error, updates the pads and
// each device, and initialises each device's port, cleared state and copy of
// the master event table. CDeviceManager and CGCDevice are named by the
// mangled symbols; the device and actuator layouts, the pad data view and the
// table's type are inferred from offsets. Init clears 12 bytes from offset 10,
// which runs into the first two bytes of the actuator at offset 20. In
// SetActuatorState the cases share one tail that fills in the actuator and
// all paths share one return, written here with a label; the actuator index
// is a local copy of the parameter (the target moves it first).
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

struct CGCActuatorView {
    long type;
    int duration;
    float level;
    float time;
    float unknown0c;
    float unknown10;
    bool active;
};

class CGCDevice {
public:
    void UpdateActuators(float);
    void Update();

    bool m_ready;
    unsigned char unknown01[3];
    int m_port;
    unsigned char m_state[2];
    unsigned char m_cleared[10];
    CGCActuatorView m_actuators[1];
    unsigned char m_events[2268];
};

class CDeviceManager {
public:
    CDeviceManager();
    ~CDeviceManager();
    void UpdateActuators(float);
    bool SetActuatorState(int, long, long, float, float);
    void* GetDeviceState(unsigned long);
    bool IsReady(int);
    void Update();
    void Shutdown();
    void Init();

    CGCDevice m_devices[4];
};

void CDeviceManager::UpdateActuators(float time) {
    for (int i = 0; i < 4; i++)
        m_devices[i].UpdateActuators(time);
}

bool CDeviceManager::SetActuatorState(int device, long type, long actuator, float level, float duration) {
    long index = actuator;
    switch (type) {
    case 0:
        if (index >= 1)
            break;
        m_devices[device].m_actuators[index].level = 0.0f;
        goto set;
    case 1:
    case 2:
        index = 0;
        m_devices[device].m_actuators[index].level = level;
        goto set;
    case 9:
        index = 0;
        m_devices[device].m_actuators[index].level = 100.0f;
        goto set;
    case 10:
        index = 0;
        m_devices[device].m_actuators[index].level = 177.0f;
        goto set;
    case 11:
    case 12:
        index = 0;
        m_devices[device].m_actuators[index].level = 255.0f;
        goto set;
    case 13:
        if (index >= 1)
            break;
        m_devices[device].m_actuators[index].level = (unsigned int)(255.0f * level);
    set:
        m_devices[device].m_actuators[index].type = type;
        m_devices[device].m_actuators[index].duration = duration;
        m_devices[device].m_actuators[index].time = duration;
        m_devices[device].m_actuators[index].unknown0c = 0.0f;
        m_devices[device].m_actuators[index].unknown10 = 0.0f;
        m_devices[device].m_actuators[index].active = true;
        break;
    }
    return true;
}

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
