// Alarm built-in script functions, the first two of the file: setting the
// alarm-activated flag from the first argument and pushing it back. The
// globals are named by the symbols; the built-in record view (16 bytes,
// argument count at +10, the first argument's depth at +12) is inferred, as
// in bsbifunc.cpp and bsmachin.cpp. The rest of the file is not part of this
// unit.
struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern bool g_alarm_activated;

void BIFunc_SetAlarmActivated(int** stack, void*) {
    BSBuiltinView* builtin = &g_pBuiltInFunctions[g_iCurrentBIFIndex];
    g_alarm_activated = *(*stack - (builtin->firstArgument - 1)) != 0;
    *stack -= builtin->argumentCount;
}

void BIFunc_IsAlarmActivated(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_alarm_activated;
}
