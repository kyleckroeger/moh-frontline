#include <dolphin.h>
#include <dolphin/db.h>
#include <dolphin/os.h>

#include "__os.h"

#define HID2 920

// prototypes
void DMAErrorHandler(OSError error, OSContext* context, ...);

asm void DCFlashInvalidate(void) {
  nofralloc
  mfspr r3, HID0
  ori r3, r3, 0x400
  mtspr HID0, r3
  blr
}

asm void DCEnable(void) {
  nofralloc
  sync
  mfspr r3, HID0
  ori   r3, r3, 0x4000
  mtspr HID0, r3
  blr
}

asm void DCDisable(void) {
  nofralloc
  sync
  mfspr r3, HID0
  rlwinm r3, r3, 0, 18, 16
  mtspr HID0, r3
  blr
}

asm void DCFreeze(void) {
  nofralloc
  sync
  mfspr r3, HID0
  ori r3, r3, 0x1000
  mtspr HID0, r3
  blr
}

asm void DCUnfreeze(void) {
  nofralloc
  mfspr r3, HID0
  rlwinm r3, r3, 0, 20, 18
  mtspr HID0, r3
  blr
}

asm void DCTouchLoad(register void * addr) {
  nofralloc
  dcbt r0, addr
  blr
}

asm void DCBlockZero(register void * addr) {
  nofralloc
  dcbz r0, addr
  blr
}

asm void DCBlockStore(register void * addr) {
  nofralloc
  dcbst r0, addr
  blr
}

asm void DCBlockFlush(register void * addr) {
  nofralloc
  dcbf r0, addr
  blr
}

asm void DCBlockInvalidate(register void * addr) {
  nofralloc
  dcbi r0, addr
  blr
}

asm void DCInvalidateRange(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi   nBytes, nBytes, 32
@2
  addi   nBytes, nBytes, 31
  srwi   nBytes, nBytes, 5
  mtctr  nBytes

@1
  dcbi r0, addr
  addi addr, addr, 32
  bdnz @1
  blr
}


asm void DCFlushRange(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbf r0, addr
  addi addr, addr, 32
  bdnz @1
  sc
  blr
}

asm void DCStoreRange(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbst r0, addr
  addi addr, addr, 32
  bdnz @1
  sc

  blr
}

asm void DCFlushRangeNoSync(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbf r0, addr
  addi addr, addr, 32
  bdnz @1
  blr
}


asm void DCStoreRangeNoSync(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbst r0, addr
  addi addr, addr, 32
  bdnz @1

  blr
}

asm void DCZeroRange(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbz r0, addr
  addi addr, addr, 32
  bdnz @1

  blr
}

asm void DCTouchRange(register void * addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  dcbt r0, addr
  addi addr, addr, 32
  bdnz @1

  blr
}

asm void ICInvalidateRange(register void* addr, register u32 nBytes) {
  nofralloc
  cmplwi nBytes, 0
  blelr
  clrlwi. r5, addr, 27
  beq @2
  addi nBytes, nBytes, 32
@2
  addi nBytes, nBytes, 31
  srwi nBytes, nBytes, 5
  mtctr nBytes

@1
  icbi r0, addr
  addi addr, addr, 32
  bdnz @1
  sync
  isync

  blr
}


asm void ICFlashInvalidate(void) {
  nofralloc
  mfspr r3, HID0
  ori r3, r3, 0x800
  mtspr HID0, r3
  blr
}

asm void ICEnable(void) {
  nofralloc
  isync
  mfspr r3, HID0
  ori r3, r3, 0x8000
  mtspr HID0, r3
  blr
}

asm void ICDisable(void) {
  nofralloc
  isync
  mfspr r3, HID0
  rlwinm r3, r3, 0, 17, 15
  mtspr HID0, r3
  blr
}

asm void ICFreeze(void) {
  nofralloc
  isync
  mfspr r3, HID0
  ori r3, r3, 0x2000
  mtspr HID0, r3
  blr
}

asm void ICUnfreeze(void) {
  nofralloc
  mfspr r3, HID0
  rlwinm r3, r3, 0, 19, 17
  mtspr HID0, r3
  blr
}

asm void ICBlockInvalidate(register void * addr) {
  nofralloc
  icbi r0, addr
  blr
}

asm void ICSync(void) {
  nofralloc
  isync
  blr
}

#define LC_LINES    512
#define CACHE_LINES 1024

static asm void __LCEnable(void) {
  nofralloc
  mfmsr   r5
  ori     r5, r5, 0x1000
  mtmsr   r5

  lis     r3, OS_CACHED_REGION_PREFIX
  li      r4, CACHE_LINES
  mtctr   r4
_touchloop:
  dcbt    0,r3
  dcbst   0,r3
  addi    r3,r3,32
  bdnz    _touchloop
  mfspr   r4, HID2
  oris    r4, r4, 0x100F
  mtspr   HID2, r4

  nop 
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  lis     r3, LC_BASE_PREFIX
  ori     r3, r3, 0x0002
  mtspr   DBAT3L, r3
  ori     r3, r3, 0x01fe
  mtspr   DBAT3U, r3
  isync
  lis     r3, LC_BASE_PREFIX
  li      r6, LC_LINES
  mtctr   r6
  li      r6, 0

_lockloop:
  dcbz_l  r6, r3
  addi    r3, r3, 32
  bdnz+    _lockloop

  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop

  blr
}

void LCEnable(void) {
  BOOL enabled;

  enabled = OSDisableInterrupts();
  __LCEnable();
  OSRestoreInterrupts(enabled);
}


asm void LCDisable(void) {
  nofralloc
  lis     r3, LC_BASE_PREFIX
  li      r4, LC_LINES
  mtctr r4
@1
  dcbi r0, r3
  addi r3, r3, 32
  bdnz @1
  mfspr r4, HID2
  rlwinm r4, r4, 0, 4, 2
  mtspr HID2, r4
  blr
}

static void L2Init(void) {
    u32 oldMSR;
    oldMSR = PPCMfmsr();
    __sync();
    PPCMtmsr(MSR_IR | MSR_DR);
    __sync();
    L2Disable();
    L2GlobalInvalidate();
    PPCMtmsr(oldMSR);
}

void L2Enable(void) { 
    PPCMtl2cr((PPCMfl2cr() | L2CR_L2E) & ~L2CR_L2I);
}

void L2Disable(void) {
    __sync();
    PPCMtl2cr(PPCMfl2cr() & ~0x80000000);
    __sync();
}

void L2GlobalInvalidate(void) {
    L2Disable();
    PPCMtl2cr(PPCMfl2cr() | 0x00200000);
    while (PPCMfl2cr() & 0x00000001u);

    PPCMtl2cr(PPCMfl2cr() & ~0x00200000);
    while (PPCMfl2cr() & 0x00000001u) {
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}

void L2SetDataOnly(BOOL dataOnly) {
    if (dataOnly) {
        PPCMtl2cr(PPCMfl2cr() | 0x400000);
        return;
    }
    PPCMtl2cr(PPCMfl2cr() & 0xFFBFFFFF);
}

void L2SetWriteThrough(BOOL writeThrough) {
    if (writeThrough) {
        PPCMtl2cr(PPCMfl2cr() | 0x80000);
        return;
    }
    PPCMtl2cr(PPCMfl2cr() & 0xFFF7FFFF);
}

void DMAErrorHandler(OSError error, OSContext* context, ...) {
    u32 hid2 = PPCMfhid2();

    OSReport("Machine check received\n");
    OSReport("HID2 = 0x%x   SRR1 = 0x%x\n", hid2, context->srr1);
    if (!(hid2 & (HID2_DCHERR | HID2_DNCERR | HID2_DCMERR | HID2_DQOERR)) || !(context->srr1 & SRR1_DMA_BIT)) {
        OSReport("Machine check was not DMA/locked cache related\n");
        OSDumpContext(context);
        PPCHalt();
    }

    OSReport("DMAErrorHandler(): An error occurred while processing DMA.\n");
    OSReport("The following errors have been detected and cleared :\n");

    if (hid2 & HID2_DCHERR) {
        OSReport("\t- Requested a locked cache tag that was already in the cache\n");
    }

    if (hid2 & HID2_DNCERR) {
        OSReport("\t- DMA attempted to access normal cache\n");
    }

    if (hid2 & HID2_DCMERR) {
        OSReport("\t- DMA missed in data cache\n");
    }

    if (hid2 & HID2_DQOERR) {
        OSReport("\t- DMA queue overflowed\n");
    }

    // write hid2 back to clear the error bits
    PPCMthid2(hid2);
}

void __OSCacheInit() {
    if (!(PPCMfhid0() & HID0_ICE)) {
        ICEnable();
        DBPrintf("L1 i-caches initialized\n");
    }

    if (!(PPCMfhid0() & HID0_DCE)) {
        DCEnable();
        DBPrintf("L1 d-caches initialized\n");
    }

    if (!(PPCMfl2cr() & L2CR_L2E)) {
        L2Init();
        L2Enable();
        DBPrintf("L2 cache initialized\n");
    }

    OSSetErrorHandler(OS_ERROR_MACHINE_CHECK, DMAErrorHandler);
    DBPrintf("Locked cache machine check handler installed\n");
}
