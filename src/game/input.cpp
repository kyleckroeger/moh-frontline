// The static initialisation of input.cpp (0x80085230): the global input
// manager is built through its device manager's constructor and its
// destructor registered, then the weak CInputManager destructor (the device
// manager destroyed as a member), emitted for the registration. The rest of the file is in other units. g_inputMgr,
// CInputManager and CDeviceManager are named by the symbols; the object size
// comes from the symbol; the device manager is a member (its destructor is
// called for a complete object), and the rest of the layout is opaque here.
class CDeviceManager {
public:
    CDeviceManager();
    ~CDeviceManager();

    unsigned char unknown0000[9296];
};

class CInputManager {
public:
    ~CInputManager() {}

    CDeviceManager m_devices;
};

CInputManager g_inputMgr;
