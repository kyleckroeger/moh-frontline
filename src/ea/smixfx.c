// Mixer effect set-up: clear the effect state and, for a version 10 header,
// count the filters in the module list. sndfx's layout and the module list
// records are not known; they are accessed through inferred views.
extern "C" void* memset(void*, int, unsigned long);
extern "C" char sndfx[];
extern int filtermodsizetable[];

struct FXMODULEVIEW {
    unsigned char type;
    unsigned char field01[19];
};

extern FXMODULEVIEW* plisthead;

static int FXfilterindex;

extern "C" void SNDMIXI_fxinit(unsigned char* header) {
    FXMODULEVIEW* module;

    *(int*)(sndfx + 1920) = 3000;
    memset(sndfx, 0, 640);
    if (header[2] == 10) {
        FXfilterindex = 0;
        module = plisthead;
        while (module->type) {
            module += filtermodsizetable[module->type];
            FXfilterindex++;
        }
    }
}

// SNDMIXI_modlapifxadd, SNDMIXI_fxadd and MIX_getwetbuffer follow in the
// original file and are not reconstructed.
