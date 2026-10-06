// RCMP start-up and shutdown: initialise the global framework and set the
// RCMP system's REAL defaults, or restore the framework. FRAMEWORK, g_fw,
// RCMP and RCMP_SYSTEM are named by the symbols; the framework object is an
// opaque view here. The rest of the file is not part of this unit.
class FRAMEWORK {
public:
    void Init();
    void Restore();

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

extern FRAMEWORK g_fw;

void RCMP_Shutdown() {
    g_fw.Restore();
}

void RCMP_Initialize() {
    g_fw.Init();
    RCMP::rcmp_sys.SetREALDefaults();
}
