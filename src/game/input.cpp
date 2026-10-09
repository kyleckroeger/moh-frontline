// The static initialisation of input.cpp (0x80085230): the global input
// manager is built through CDeviceManager's constructor and its destructor
// registered. The rest of the file is in other units. g_inputMgr,
// CInputManager and CDeviceManager are named by the symbols; the object size
// comes from the symbol, the members are opaque here.
class CDeviceManager {
public:
    CDeviceManager();

    unsigned char unknown0000[9296];
};

class CInputManager : public CDeviceManager {
public:
    ~CInputManager();
};

CInputManager g_inputMgr;
