//
// __ppc_eabi_init
//

#include "global.h"
#include "dolphin/base/PPCArch.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Forward References:
//

void __init_user();
static void __init_cpp();
void _ExitProcess();
void __OSPSInit(void);
void __OSCacheInit(void);

//
// External References:
//

typedef void (*voidfunctionptr)(); // pointer to function returning void
extern voidfunctionptr _ctors[];

__declspec(section ".init") asm void __init_hardware(void)
{ // clang-format off
    nofralloc
    mfmsr r0
    ori  r0,r0,MSR_FP
    mtmsr r0
    mflr    r31
    bl      __OSPSInit
    bl      __OSCacheInit
    mtlr    r31
    blr
}

__declspec(section ".init") asm void __flush_cache(void *address, unsigned int size)
{ // clang-format off
    nofralloc
    lis     r5, 0xffff
    ori     r5, r5, 0xfff1
    and     r5, r5, r3
    subf    r3, r5, r3
    add     r4, r4, r3
rept:
    dcbst   0,r5
    sync
    icbi    0,r5
    addic   r5,r5,0x8
    subic.  r4,r4,0x8
    bge     rept
    isync
    blr
}

void __init_user(void) {
    __init_cpp();
}

#pragma peephole off
static void __init_cpp(void) {
    /**
     *	call static initializers
     */
    voidfunctionptr* constructor;
    for (constructor = _ctors; *constructor; constructor++) {
        (*constructor)();
    }
}
#pragma peephole reset

void _ExitProcess(void) {
    PPCHalt();
}

#ifdef __cplusplus
}
#endif