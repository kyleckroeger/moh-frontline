// The animation module's start-up, user-opcode callbacks and the global
// animation database. Callback signatures are inferred from the calls; their
// return values are passed through.
class CAnimDatabase {
public:
    CAnimDatabase();
    ~CAnimDatabase();
    void Init();

private:
    // Only the size (176 bytes, from the g_AnimDB symbol) is established.
    unsigned char unknown00[176];
};

struct AnimHeaderView {
    short field00;
    short field02;
    short field04;
};

typedef int (*AnimUserOpcodeCallback)(void*, unsigned short, unsigned long);

extern "C" void SysSetLastErrorFunc(int);

static bool _Anim_ModuleActive;
static AnimUserOpcodeCallback _Anim_pUserOpcodeCallback;
static AnimUserOpcodeCallback _Anim_pUserOpcodeSkipCallback;
static void* _Anim_pCurrentUserData[3];
static unsigned long _Anim_UserCallbackState[3];
static signed char _Anim_UserDataStackPosition;
static signed char _Anim_UserStateStackPosition;

CAnimDatabase g_AnimDB;

extern "C" {

void AnimSetUserOpcodeCallbacks(AnimUserOpcodeCallback callback, AnimUserOpcodeCallback skipCallback) {
    _Anim_pUserOpcodeCallback = callback;
    _Anim_pUserOpcodeSkipCallback = skipCallback;
}

void AnimInitHeader(AnimHeaderView* header, short a, short b, short c) {
    header->field00 = a;
    header->field02 = b;
    header->field04 = c;
}

int AnimShutdown(void) {
    int result = 0;
    if (_Anim_ModuleActive) {
        _Anim_pUserOpcodeCallback = 0;
        _Anim_ModuleActive = false;
    } else {
        result = 0x140002;
    }
    SysSetLastErrorFunc(result);
    return result;
}

int AnimInit(void) {
    int result = 0;
    bool active = _Anim_ModuleActive;
    if (!active)
        _Anim_ModuleActive = true;
    if (active)
        result = 0x140001;
    _Anim_UserDataStackPosition = -1;
    _Anim_UserStateStackPosition = -1;
    g_AnimDB.Init();
    SysSetLastErrorFunc(result);
    return result;
}
}

void _AnimUserCallbackPopInfo() {
    _Anim_UserDataStackPosition--;
}

void _AnimUserCallbackPopState() {
    _Anim_UserStateStackPosition--;
}

void _AnimUserCallbackPushInfo(void* data) {
    _Anim_UserDataStackPosition++;
    _Anim_pCurrentUserData[_Anim_UserDataStackPosition] = data;
}

void _AnimUserCallbackPushState(unsigned long state) {
    _Anim_UserStateStackPosition++;
    _Anim_UserCallbackState[_Anim_UserStateStackPosition] = state;
}

int _AnimDoUserOpcodeSkipCallback(unsigned short opcode) {
    return _Anim_pUserOpcodeSkipCallback(0, opcode, 0);
}

int _AnimDoUserOpcodeCallback(unsigned short opcode) {
    return _Anim_pUserOpcodeCallback(_Anim_pCurrentUserData[_Anim_UserDataStackPosition], opcode,
                                     _Anim_UserCallbackState[_Anim_UserDataStackPosition]);
}
