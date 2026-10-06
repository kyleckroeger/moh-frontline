// Installs the main-CPU mixer entry points. The pointers' types are not
// established here; they are stored as addresses.
extern "C" {
void MIX_create();
void MIX_destroy();
void MIX_audioslice();
void MIX_playinit();
void MIX_play();
void MIX_stop();
void MIX_setpitch();
extern void* MIXinitfn;
extern void* MIXrestorefn;
extern void* MIXaudioslicefn;
extern void* MIXplayinitfn;
extern void* MIXplayfn;
extern void* MIXstopfn;
extern void* MIXsetpitchfn;

void SNDSYS_linkmaincpumixer() {
    MIXinitfn = (void*)MIX_create;
    MIXrestorefn = (void*)MIX_destroy;
    MIXaudioslicefn = (void*)MIX_audioslice;
    MIXplayinitfn = (void*)MIX_playinit;
    MIXplayfn = (void*)MIX_play;
    MIXstopfn = (void*)MIX_stop;
    MIXsetpitchfn = (void*)MIX_setpitch;
}
}
