// A fragment of bsbifunc.cpp (0x80023f28): the script built-ins that select
// an ambient track and set the AEMS master volume (the argument scaled by
// the shell menu's sound-effect setting, 1 to 7, as 0 to 1; its constants
// are entries of the file's .sdata2 pool). It reads its argument below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around it
// are not reconstructed. The functions and globals are named by the mangled
// symbols; the built-in record and shell views are inferred.
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

void AEMS_SetVolume(float);

// The shell menu's sound-effect volume setting (1-7) at +40 (inferred view).
struct ShellAudioView {
    unsigned char unknown00[40];
    unsigned char m_sfxVolume;
    unsigned char unknown29[6563];
};

extern ShellAudioView g_Shell;

void BIFunc_AmbientTrack_Select(int** stack, void*) {
    AmbientTrack_Select(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AEMSSetMasterVolume(int** stack, void*) {
    float volume = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    AEMS_SetVolume(volume * (g_Shell.m_sfxVolume - 1) / 6.0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
