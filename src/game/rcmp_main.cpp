// RCMP start-up and shutdown: initialise the global framework and set the
// RCMP system's REAL defaults, or restore the framework. FRAMEWORK, g_fw,
// RCMP and RCMP_SYSTEM are named by the symbols; the framework object is an
// opaque view here. The unit ends with the static initialisation, which
// registers g_fw's destructor, and that weak empty destructor (emitted for
// the registration); the rest of the file is not part of it.
class FRAMEWORK {
public:
    void Init();
    void Restore();
    ~FRAMEWORK() {}

    int m_state;
};

namespace RCMP {
class RCMP_SYSTEM {
public:
    void SetREALDefaults();

    unsigned char data[16];
};

extern RCMP_SYSTEM rcmp_sys;
}

FRAMEWORK g_fw;

void RCMP_Shutdown() {
    g_fw.Restore();
}

void RCMP_Initialize() {
    g_fw.Init();
    RCMP::rcmp_sys.SetREALDefaults();
}
