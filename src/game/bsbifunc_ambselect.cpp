// A fragment of bsbifunc.cpp (0x80023f28): the script built-in that selects an
// ambient track. It reads its argument below the script stack top and pops the
// built-in's arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around it are not reconstructed. The functions
// and globals are named by the mangled symbols; the built-in record view is
// inferred.
void AmbientTrack_Select(int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_AmbientTrack_Select(int** stack, void*) {
    AmbientTrack_Select(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
