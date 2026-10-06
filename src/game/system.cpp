// EA system module start-up and shutdown. Only the size of the critical
// section object (36 bytes) is established.
struct SysInitParms_t;

struct SysCriticalSectionView {
    unsigned char unknown00[36];
};

int SysInitDependent(const SysInitParms_t*);
int SysShutdownDependent();

extern "C" {
void SysInitCriticalSectionFunc(SysCriticalSectionView*);
void SysShutdownCriticalSectionFunc(SysCriticalSectionView*);

bool _Sys_ModuleActive;
static int _Sys_LastError;
static SysCriticalSectionView _SysCS;

void SysSetLastErrorFunc(int error) {
    _Sys_LastError = error;
}

int SysShutdown(void) {
    int result;
    if (_Sys_ModuleActive) {
        result = SysShutdownDependent();
        SysShutdownCriticalSectionFunc(&_SysCS);
        _Sys_ModuleActive = false;
    } else {
        result = 0x20002;
    }
    _Sys_LastError = result;
    return result;
}

int SysInit(const SysInitParms_t* parms) {
    int result;
    if (!_Sys_ModuleActive) {
        result = SysInitDependent(parms);
        _Sys_ModuleActive = true;
        SysInitCriticalSectionFunc(&_SysCS);
    } else {
        result = 0x20001;
    }
    _Sys_LastError = result;
    return result;
}
}
