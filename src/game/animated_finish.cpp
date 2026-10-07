// A fragment of animated.cpp (0x80061384): CAnimated::FinishAnimationState
// finishes the animation recorded for a state (read from the state machine's
// user memory, unless it is -1) on the animation object, passing the state's
// top bit as the flag, and returns 0. CAnimated, CAnimObject and the
// functions are named by the mangled symbols; the members and the result type
// are inferred (CAnimated is a non-virtual view). The rest of the file is not
// part of this unit.
extern "C" void* AnimStGetStateUserMemory(void*, unsigned short);

class CAnimObject {
public:
    void FinishAnimationState(long, bool);
};

class CAnimated {
public:
    int FinishAnimationState(unsigned short);

    unsigned char unknown0000[2192];
    unsigned char m_stateMachine[1064];
    CAnimObject* m_animObject;
};

int CAnimated::FinishAnimationState(unsigned short state) {
    long* animation = (long*)AnimStGetStateUserMemory(m_stateMachine, state);

    if (*animation != -1)
        m_animObject->FinishAnimationState(*animation, (state & 0x8000) != 0);
    return 0;
}
