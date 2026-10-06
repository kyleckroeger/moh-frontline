// A fragment of bsbifunc.cpp (0x80029a3c): script built-ins that send a music
// (pathfinder) event and set the music level. Each reads its argument below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. The functions and globals are named by the mangled
// symbols; the built-in record view is inferred.
void MUSIC_SendEvent(int);
void MUSIC_SetLevel(int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PathfinderEvent(int** stack, void*) {
    MUSIC_SendEvent(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PathfinderSetLevel(int** stack, void*) {
    MUSIC_SetLevel(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
