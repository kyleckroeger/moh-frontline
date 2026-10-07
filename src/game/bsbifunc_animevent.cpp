// A fragment of bsbifunc.cpp (0x800209c0): the script built-in that sets
// whether the calling animated object delivers an event when a single
// animation ends (a flag at +9298). It reads its argument below the script
// stack top and pops the built-in's arguments. BIFunc_SetDetonationTime
// after it sets a thrown bullet's detonation time to 60 times its argument
// (60.0f is an entry of the file's .sdata2 pool). The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around it
// are not reconstructed. The function and globals are named by the mangled
// symbols; the animated-object and built-in record views are inferred.
class CAnimObject {
public:
    unsigned char unknown0000[9298];
    bool m_singleAnimEndsEvent;
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

class CThrownBullet {
public:
    void SetDetonationTime(float);
};

void BIFunc_SetSingleAnimEndsEventDelivery(int** stack, void* object) {
    ((CAnimObject*)object)->m_singleAnimEndsEvent = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetDetonationTime(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    CThrownBullet* bullet = (CThrownBullet*)*(*stack - (count - 1));
    float time = *(float*)(*stack - (count - 2));
    bullet->SetDetonationTime(60.0f * time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
