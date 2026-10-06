// A fragment of bsbifunc.cpp (0x80029ee0): the script built-in that ends a
// sound (removing its handle from the sound schedule unless it is -1). It reads
// its argument below the script stack top and pops the built-in's arguments.
// The file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around it are not reconstructed. The functions and globals are
// named by the mangled symbols; the built-in record view is inferred.
class SoundSchedule {
public:
    void RemoveSound(int, bool);
};

extern SoundSchedule g_SoundSchedule;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_EndSound(int** stack, void*) {
    int handle = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (handle != -1)
        g_SoundSchedule.RemoveSound(handle, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
