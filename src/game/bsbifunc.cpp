// Built-in script functions that only drop their arguments: stopping,
// continuing and starting orientation towards a cover point pop the current
// built-in's argument count from the script stack. The globals are named by
// the symbols; the built-in record view (16 bytes, argument count at +10) is
// inferred, as in bsmachin.cpp. The rest of the file is not part of this unit.
struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    unsigned char unknown0c[4];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_AIStopOrientTowardsCoverPoint(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIContinueOrientTowardsCoverPoint(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStartOrientTowardsCoverPoint(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
