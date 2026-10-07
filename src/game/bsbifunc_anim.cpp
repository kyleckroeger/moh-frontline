// A fragment of bsbifunc.cpp (0x8002fb28): the animation built-ins.
// PlayAnimationOnOther sets an animation state on the object given as the
// fourth argument; PlayAnimationNoTranslation and PlayLocomotion set the two
// movement flags at +9304 and +9305 (off or on) and set the state on the
// built-in's object, queueing it when the state could not be set (-65536
// back) and the script data's first word is not positive. Each reads its
// arguments below the script stack top and pops the built-in's arguments;
// the rate, 1.0f, is an entry of the file's .sdata2 pool. Between them is
// the weak CAnimObject::GetScriptObject (the word at +9104). The file name
// is this project's; the original record is bsbifunc.cpp, and PlayAnimation
// after these is not reconstructed. The functions and classes are named by
// the mangled symbols; CAnimObject is declared with its virtual functions up
// to GetScriptObject (+96 in __vt__11CAnimObject; the earlier slots are
// placeholders named by offset), and the script object, script data and
// built-in record views are inferred (members at their offsets, names not
// original).
struct PlayerScriptDataView {
    int m_value00;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual PlayerScriptDataView* GetScriptData();
};

class BSObject {
public:
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

class CAnimObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual BSObject* GetScriptObject() const;

    long SetAnimationState(unsigned short, float, long, void*);
    void QueueAnimationState(unsigned short, float, long, void*);

    unsigned char unknown0004[9100];
    BSObject* m_scriptObject;
    unsigned char unknown2394[196];
    bool m_flag2458;
    bool m_flag2459;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayAnimationOnOther(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    unsigned short state = *(*stack - (count - 1));
    int value = *(*stack - (count - 2));
    void* data = (void*)*(*stack - (count - 3));
    CAnimObject* object = (CAnimObject*)*(*stack - (count - 4));
    object->SetAnimationState(state, 1.0f, value, data);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayAnimationNoTranslation(int** stack, void* user) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int state = *(*stack - (count - 1));
    int value = *(*stack - (count - 2));
    void* data = (void*)*(*stack - (count - 3));
    ((CAnimObject*)user)->m_flag2458 = false;
    ((CAnimObject*)user)->m_flag2459 = false;
    if (((CAnimObject*)user)->SetAnimationState(state, 1.0f, value, data) == -65536 &&
        ((CAnimObject*)user)->GetScriptObject()->user->GetScriptData()->m_value00 <= 0)
        ((CAnimObject*)user)->QueueAnimationState(state, 1.0f, value, data);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) BSObject* CAnimObject::GetScriptObject() const {
    return m_scriptObject;
}

void BIFunc_PlayLocomotion(int** stack, void* user) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int state = *(*stack - (count - 1));
    int value = *(*stack - (count - 2));
    void* data = (void*)*(*stack - (count - 3));
    ((CAnimObject*)user)->m_flag2458 = true;
    ((CAnimObject*)user)->m_flag2459 = true;
    if (((CAnimObject*)user)->SetAnimationState(state, 1.0f, value, data) == -65536 &&
        ((CAnimObject*)user)->GetScriptObject()->user->GetScriptData()->m_value00 <= 0)
        ((CAnimObject*)user)->QueueAnimationState(state, 1.0f, value, data);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
