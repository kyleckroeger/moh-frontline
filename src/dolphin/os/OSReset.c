#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

// These macros are copied from OSThread.c. Or ARE they the same
// macros? They dont seem to be in the SDK headers.
#define ENQUEUE_INFO(info, queue)                            \
    do {                                                     \
        OSResetFunctionInfo* __prev = (queue)->tail; \
        if (__prev == 0) {                                   \
            (queue)->head = (info);                          \
        } else {                                             \
            __prev->next = (info);                           \
        }                                                    \
        (info)->prev = __prev;                               \
        (info)->next = 0;                                    \
        (queue)->tail = (info);                              \
    } while(0);

#define DEQUEUE_INFO(info, queue)                           \
    do {                                                    \
        OSResetFunctionInfo* __next = (info)->next; \
        OSResetFunctionInfo* __prev = (info)->prev; \
        if (__next == 0) {                                  \
            (queue)->tail = __prev;                         \
        } else {                                            \
            __next->prev = __prev;                          \
        }                                                   \
        if (__prev == 0) {                                  \
            (queue)->head = __next;                         \
        } else {                                            \
            __prev->next = __next;                          \
        }                                                   \
    } while(0);

#define ENQUEUE_INFO_PRIO(info, queue)               \
    do {                                             \
        OSResetFunctionInfo* __prev;         \
        OSResetFunctionInfo* __next;         \
        for(__next = (queue)->head; __next           \
          && (__next->priority <= (info)->priority); \
                __next = __next->next) ;             \
                                                     \
        if (__next == 0) {                           \
            ENQUEUE_INFO(info, queue);               \
        } else {                                     \
            (info)->next = __next;                   \
            __prev = __next->prev;                   \
            __next->prev = (info);                   \
            (info)->prev = __prev;                   \
            if (__prev == 0) {                       \
                (queue)->head = (info);              \
            } else {                                 \
                __prev->next = (info);               \
            }                                        \
        }                                            \
    } while(0);

static OSResetFunctionQueue ResetFunctionQueue;


// prototypes



void OSRegisterResetFunction(OSResetFunctionInfo* info) {
    ASSERTLINE(208, info->func);

    ENQUEUE_INFO_PRIO(info, &ResetFunctionQueue);
}

void OSUnregisterResetFunction(OSResetFunctionInfo* info) {
    DEQUEUE_INFO(info, &ResetFunctionQueue);
}

int __OSCallResetFunctions(BOOL final) {
    OSResetFunctionInfo* info;
    int err = 0;
    for (info = ResetFunctionQueue.head; info != 0; info = info->next) {
        err |= !info->func(final);
    }
    err |= !__OSSyncSram();
    if (err) return 0;
    return 1;
}

// The functions below follow an SDK revision between dolsdk2001 and
// dolsdk2004 (reconstructed for this target): Reset is dolsdk2001's;
// KillThreads and __OSDoHotReset are dolsdk2004's; OSResetSystem shuts down
// the devices inline (no __OSShutdownDevices), restores the pad
// recalibration setting at the end, and OSGetResetCode returns 0x80000000
// after a reboot. __OSCallResetFunctions walks the whole queue (as in
// dolsdk2001).
static asm void Reset(register u32 resetCode) {
    nofralloc
    b L_000001BC
L_000001A0:
    mfspr r8, HID0
    ori r8, r8, 0x8
    mtspr HID0, r8
    isync
    sync
    nop
    b L_000001C0
L_000001BC:
    b L_000001DC
L_000001C0:
    mftb r5, 268
L_000001C4:
    mftb r6, 268
    subf r7, r5, r6
    cmplwi r7, 0x1124
    blt L_000001C4
    nop
    b L_000001E0
L_000001DC:
    b L_000001FC
L_000001E0:
    lis r8, 0xcc00
    ori r8, r8, 0x3000
    li r4, 0x3
    stw r4, 0x24(r8)
    stw r3, 0x24(r8)
    nop
    b L_00000200
L_000001FC:
    b L_00000208
L_00000200:
    nop
    b L_00000200
L_00000208:
    b L_000001A0
}

static void KillThreads(void) {
    OSThread* thread;
    OSThread* next;

    for (thread = __OSActiveThreadQueue.head; thread; thread = next) {
        next = thread->linkActive.next;
        switch (thread->state) {
        case 1:
        case 4:
            OSCancelThread(thread);
            break;
        default:
            break;
        }
    }
}

void __OSDoHotReset(u32 resetCode) {
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
    Reset(resetCode * 8);
}

void LCDisable(void);

void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu) {
    BOOL rc;
    BOOL disableRecalibration;
    OSSram* sram;
    /* inferred: two unused locals in this revision (the original frame is 64
       bytes); their names are not known */
    BOOL enabled;
    int unused;

    OSDisableScheduler();
    __OSStopAudioSystem();
    if (reset == OS_RESET_SHUTDOWN)
        disableRecalibration = __PADDisableRecalibration(TRUE);
    while (!__OSCallResetFunctions(FALSE))
        ;
    if (reset == OS_RESET_HOTRESET && forceMenu) {
        sram = __OSLockSram();
        sram->flags |= 0x40;
        __OSUnlockSram(TRUE);
        while (!__OSSyncSram())
            ;
    }
    OSDisableInterrupts();
    rc = __OSCallResetFunctions(TRUE);
    ASSERTLINE(0x117, rc);
    LCDisable();
    if (reset == OS_RESET_HOTRESET) {
        __OSDoHotReset(resetCode);
    } else if (reset == OS_RESET_RESTART) {
        KillThreads();
        OSEnableScheduler();
        __OSReboot(resetCode, forceMenu);
    }
    KillThreads();
    memset(OSPhysicalToCached(0x40), 0, 0xcc - 0x40);
    memset(OSPhysicalToCached(0xd4), 0, 0xe8 - 0xd4);
    memset(OSPhysicalToCached(0xf4), 0, 0xf8 - 0xf4);
    memset(OSPhysicalToCached(0x3000), 0, 0xc0);
    memset(OSPhysicalToCached(0x30c8), 0, 0xd4 - 0xc8);
    __PADDisableRecalibration(disableRecalibration);
}

u32 OSGetResetCode(void) {
    if (*(u8*)OSPhysicalToCached(0x30e2))
        return 0x80000000;
    return (__PIRegs[9] & 0xFFFFFFF8) / 8;
}
