// A fragment of bsbifunc.cpp (0x80028690): script built-ins that return an
// objective's status (kept in memory before the result is written) and set it.
// Each reads its arguments below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions and globals are named by the mangled symbols; the built-in record
// view is inferred.
bool GetObjectiveStatus(unsigned int);
void SetObjectiveStatus(unsigned int, bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_GetObjective(int** stack, void*) {
    int value = GetObjectiveStatus(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_SetObjective(int** stack, void*) {
    SetObjectiveStatus(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)), *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2)) != 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
