// REAL print channels, the functions at the end of the file: formatting a
// message for a channel and passing it to each enabled output device, the C
// and C++ variadic entry points (the latter on channel 2), and the device and
// channel enable switches (which initialise the print system first).
// PRINTCHANNEL and the va_list tag are named by the mangled symbols (va_list is
// MSL's std::__tag_va_List array); the channel and device records are inferred
// views. PRINT_movechannel and PRINT_init come first in the file and are not
// part of this unit.
namespace std {
struct __tag_va_List {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
};
typedef __tag_va_List va_list[1];
}

extern "C" int vsprintf(char*, const char*, std::va_list);

enum PRINTCHANNEL {};

struct PrintChannelView {
    unsigned char unknown00[4];
    unsigned char enabled : 1;
    unsigned char flags : 7;
    unsigned char unknown05[3];
};

struct PrintDeviceView {
    unsigned char unknown00[4];
    void (*print)(PRINTCHANNEL, const char*);
    unsigned char enabled : 1;
    unsigned char flags : 7;
    unsigned char unknown09[3];
};

extern PrintChannelView PRINTchannellist[];
extern PrintDeviceView PRINTdevicelist[];
extern int PRINTinitialized;

extern "C" void PRINT_init(void);
void PRINT_movechannel(PRINTCHANNEL, int);

static void PRINT_vstring(PRINTCHANNEL channel, const char* format, std::va_list args) {
    char buffer[8192];

    if (!PRINTinitialized)
        PRINT_init();
    if (PRINTchannellist[channel].enabled) {
        vsprintf(buffer, format, args);
        for (int i = 0; i < 8; i++) {
            if (PRINTdevicelist[i].enabled && PRINTdevicelist[i].print)
                PRINTdevicelist[i].print(channel, buffer);
        }
    }
}

extern "C" void PRINT_string(PRINTCHANNEL channel, const char* format, ...) {
    std::va_list args;

    __builtin_va_info(&args);
    PRINT_vstring(channel, format, args);
}

void PRINT_string(const char* format, ...) {
    std::va_list args;

    __builtin_va_info(&args);
    PRINT_vstring((PRINTCHANNEL)2, format, args);
}

extern "C" void PRINT_setdevicestate(int device, int state) {
    if (PRINTdevicelist[device].print) {
        if (!PRINTinitialized)
            PRINT_init();
        PRINTdevicelist[device].enabled = state;
    }
}

extern "C" void PRINT_setchannelstate(PRINTCHANNEL channel, int state) {
    if (!PRINTinitialized)
        PRINT_init();
    PRINT_movechannel(channel, state);
}
