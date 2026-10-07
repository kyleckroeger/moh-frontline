// A fragment of bsbifunc.cpp (0x80029844): built-ins that initialise a value
// to zero, convert a float argument to an integer, return the generic sound
// structure memory and set the path player's latency (through
// MUSIC_SetLatency), and fade the path player to a percentage over a time
// (PathfinderFadeVolume; 100.0f is an entry of the file's .sdata2 pool).
// Values go through a local integer/float union (inferred), copied whole to
// the top of the script stack after the arguments are popped; arguments are
// read below the stack top by the built-in's parameter count. The file name
// is this project's; the original record is bsbifunc.cpp and the built-ins
// around these are not reconstructed. The built-in record view (argument
// count at +10, parameter count at +12) and the value union are inferred.
union BSValueView {
    int i;
    float f;
};

extern unsigned char g_pGenericSoundMemory[];
bool MUSIC_SetLatency(int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void MUSIC_Fade(float, int);

void BIFunc_InitToInvalid(int** stack, void*) {
    BSValueView value;
    value.i = 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_FloatToInt(int** stack, void*) {
    BSValueView value;
    value.i = (int)*(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_GetGenericSoundStructureMemory(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = (int)g_pGenericSoundMemory;
}

void BIFunc_PathfinderSetLatency(int** stack, void*) {
    MUSIC_SetLatency(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PathfinderFadeVolume(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int level = *(*stack - (count - 1));
    int time = *(*stack - (count - 2));
    MUSIC_Fade(level / 100.0f, time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
