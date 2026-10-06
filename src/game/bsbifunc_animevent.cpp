// A fragment of bsbifunc.cpp (0x800209c0): the script built-in that sets
// whether the calling animated object delivers an event when a single animation
// ends (a flag at +9298). It reads its argument below the script stack top and
// pops the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp and the built-ins around it are not reconstructed. The
// function and globals are named by the mangled symbols; the animated-object
// and built-in record views are inferred.
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

void BIFunc_SetSingleAnimEndsEventDelivery(int** stack, void* object) {
    ((CAnimObject*)object)->m_singleAnimEndsEvent = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
