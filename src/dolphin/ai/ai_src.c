/* C fragment: original private audio sample-rate initialization. */
#include <dolphin/ai.h>
#include <dolphin/os.h>
#include <dolphin/hw_regs.h>
#include "__gx.h"
extern OSTime bound_32KHz, bound_48KHz, min_wait, max_wait, buffer;
static void __AI_SRC_INIT(void);
void __AI_SRC_INIT(void) {
    OSTime rising_32khz = 0;
    OSTime rising_48khz = 0;
    OSTime diff = 0;
    OSTime t1 = 0;
    OSTime temp;
    u32 temp0;
    u32 temp1;
    u32 done = 0;
    u32 volume = 0;
    u32 Init_Cnt = 0;
    u32 walking = 0;

    walking = 0;
    Init_Cnt = 0;
    temp = 0;

#if DEBUG
    profile.t_start = OSGetTime();
#endif

    while (!done) {
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 5, 1);
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 1, 0);
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 0, AI_STREAM_START);
        temp0 = __AIRegs[2];
        while (temp0 == __AIRegs[2]) {
        }
        rising_32khz = OSGetTime();
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 1, 1);
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 0, AI_STREAM_START);
        temp1 = __AIRegs[2];
        while (temp1 == __AIRegs[2]) {
        }
        rising_48khz = OSGetTime();
        diff = rising_48khz - rising_32khz;
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 1, 0);
        OLD_SET_REG_FIELD(0, __AIRegs[0], 1, 0, AI_STREAM_STOP);
        if (diff < bound_32KHz - buffer) {
            temp = min_wait;
            done = 1;
            Init_Cnt++;
        } else if (diff >= bound_32KHz + buffer && diff < bound_48KHz - buffer) {
            temp = max_wait;
            done = 1;
            Init_Cnt++;
        } else {
            done = 0;
            walking = 1;
            Init_Cnt++;
        }
    }
    while (rising_48khz + temp > OSGetTime()) {
    }
#if DEBUG
    profile.t_end = OSGetTime();
#endif
}

