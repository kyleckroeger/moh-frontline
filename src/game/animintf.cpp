// The animation system's start-up and shutdown, which register the user-opcode
// handlers with the animation module. CAnimCallbacks is named by the mangled
// symbols.
extern "C" {
void AnimStShutdown(void);
int AnimWgtShutdown(void);
void AnimShutdown(void);
void AnimInit(void);
int AnimWgtInit(void);
void AnimStInit(void);
}
void AnimCallback_Init();

class CAnimCallbacks {
public:
    static int _AnimIntfOpcodeSkip(void*, unsigned short, unsigned long);
    static int _AnimIntfOpcodeProcess(void*, unsigned short, unsigned long);
};

extern "C" void AnimSetUserOpcodeCallbacks(int (*)(void*, unsigned short, unsigned long),
                                           int (*)(void*, unsigned short, unsigned long));

extern "C" void AnimSystem_Shutdown(void) {
    AnimStShutdown();
    AnimWgtShutdown();
    AnimShutdown();
}

extern "C" void AnimSystem_Init(void) {
    AnimInit();
    AnimSetUserOpcodeCallbacks(CAnimCallbacks::_AnimIntfOpcodeProcess, CAnimCallbacks::_AnimIntfOpcodeSkip);
    AnimWgtInit();
    AnimStInit();
    AnimCallback_Init();
}

// CAnimCallbacks::_AnimIntfOpcodeSkip and _AnimIntfOpcodeProcess follow in
// the original file; the skip handler is drafted in scratch but not matched,
// so this unit covers the start-up and shutdown only.
