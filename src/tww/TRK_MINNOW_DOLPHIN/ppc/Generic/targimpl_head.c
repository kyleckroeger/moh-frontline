/* Frontline's target implementation, the functions at the start of the file
   record: MSR access, the file-local TRK_ppc_memcpy, the interrupt and
   exception handlers, TRKSwapAndGo, the interrupt re-enable path and
   TRKTargetSetInputPendingPtr. The bodies are Pikmin's CC0 targimpl.c
   (https://github.com/doldecomp/pikmin), compiled like targimpl.c (the tail
   fragment of the same file record, src/tww/.../Generic/targimpl.c) with the
   TWW headers plus Pikmin's DSVersions/DSCPUType types and MSR bits, at GC 1.3.
   The file name is this project's; the original record is targimpl.c. The
   other functions are declared only, and the file's data (owned by the tail
   fragment) is declared extern; its file-local objects are declared without
   static so this unit can refer to them. */
#include "TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.h"
#include "TRK_MINNOW_DOLPHIN/utils/common/MWTrace.h"
#include <stdint.h>
#include "string.h"
void TRKSaveExtended1Block();
void TRKRestoreExtended1Block();

/* MSR bits, from Pikmin's CC0 Dolphin/PPCArch.h. */
#define MSR_00  0x80000000
#define MSR_01  0x78000000
#define MSR_05  0x07c00000
#define MSR_10  0x00380000
#define MSR_POW 0x00040000
#define MSR_14  0x00020000
#define MSR_ILE 0x00010000
#define MSR_EE  0x00008000
#define MSR_PR  0x00004000
#define MSR_FP  0x00002000
#define MSR_ME  0x00001000
#define MSR_FE0 0x00000800
#define MSR_SE  0x00000400
#define MSR_BE  0x00000200
#define MSR_FE1 0x00000100
#define MSR_24  0x00000080
#define MSR_IP  0x00000040
#define MSR_IR  0x00000020
#define MSR_DR  0x00000010
#define MSR_28  0x00000008
#define MSR_PM  0x00000004
#define MSR_RI  0x00000002
#define MSR_LE  0x00000001

/* Types from Pikmin's CC0 MetroTRK headers that the TWW headers do not have. */
typedef struct DSVersions {
	u8 kernelMajor;
	u8 kernelMinor;
	u8 protocolMajor;
	u8 protocolMinor;
} DSVersions;

typedef struct DSCPUType {
	u8 cpuMajor;
	u8 cpuMinor;
	u8 bigEndian;
	u8 defaultTypeSize;
	u8 fpTypeSize;
	u8 extended1TypeSize;
	u8 extended2TypeSize;
} DSCPUType;

typedef struct memRange {
	u8* start;
	u8* end;
	BOOL readable;
	BOOL writeable;
} memRange;

extern const memRange gTRKMemMap[1];

typedef struct StopInfo_PPC {
	u32 PC;
	u32 PCInstruction;
	u16 exceptionID;
} StopInfo_PPC;
typedef struct TRKExceptionStatus {
	StopInfo_PPC exceptionInfo;
	u8 inTRK;
	u8 exceptionDetected;
} TRKExceptionStatus;
typedef struct TRKStepStatus {
	BOOL active;    // 0x0
	u8 type;        // 0x4
	u32 count;      // 0x8
	u32 rangeStart; // 0xC
	u32 rangeEnd;   // 0x10
} TRKStepStatus;

// Instruction macros
#define INSTR_NOP                                0x60000000
#define INSTR_BLR                                0x4E800020
#define DSFetch_u32(_p_)                         (*((u32*)_p_))
#define DSFetch_u64(_p_)                         (*((u64*)_p_))
#define INSTR_PSQ_ST(psr, offset, rDest, w, gqr) (0xF0000000 | (psr << 21) | (rDest << 16) | (w << 15) | (gqr << 12) | offset)
#define INSTR_PSQ_L(psr, offset, rSrc, w, gqr)   (0xE0000000 | (psr << 21) | (rSrc << 16) | (w << 15) | (gqr << 12) | offset)
#define INSTR_STW(rSrc, offset, rDest)           (0x90000000 | (rSrc << 21) | (rDest << 16) | offset)
#define INSTR_LWZ(rDest, offset, rSrc)           (0x80000000 | (rDest << 21) | (rSrc << 16) | offset)
#define INSTR_STFD(fprSrc, offset, rDest)        (0xD8000000 | (fprSrc << 21) | (rDest << 16) | offset)
#define INSTR_LFD(fprDest, offset, rSrc)         (0xC8000000 | (fprDest << 21) | (rSrc << 16) | offset)
#define INSTR_MFSPR(rDest, spr)                  (0x7C000000 | (rDest << 21) | ((spr & 0xFE0) << 6) | ((spr & 0x1F) << 16) | 0x2A6)
#define INSTR_MTSPR(spr, rSrc)                   (0x7C000000 | (rSrc << 21) | ((spr & 0xFE0) << 6) | ((spr & 0x1F) << 16) | 0x3A6)

extern ProcessorRestoreFlags_PPC gTRKRestoreFlags;
extern TRKExceptionStatus gTRKExceptionStatus;
extern TRKStepStatus gTRKStepStatus;
extern u16 TRK_saved_exceptionID;
extern ProcessorState_PPC gTRKCPUState;
extern TRKState gTRKState;

typedef unsigned char u128[16];
extern u128 TRKvalue128_temp;

extern Default_PPC gTRKSaveState;
void TRKPostInterruptEvent(void);
void TRKExceptionHandler(u16 a);
void TRKUARTInterruptHandler(void);
void TRKInterruptHandlerEnableInterrupts(void);
DSError TRKPPCAccessSPR(void*, u32, BOOL);
DSError TRKPPCAccessPairedSingleRegister(void*, u32, BOOL);
DSError TRKPPCAccessFPRegister(void*, u32, BOOL);
DSError TRKPPCAccessSpecialReg(void*, u32*, BOOL);

static BOOL TRKTargetCheckStep();

ASM u32 __TRK_get_MSR() {
#ifdef __MWERKS__ // clang-format off
    nofralloc
    mfmsr  r3
    blr
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
ASM void __TRK_set_MSR(register u32 msr) {
#ifdef __MWERKS__ // clang-format off
	nofralloc
	mtmsr  msr
	blr
#endif // clang-format on
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_PVR(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT0U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT0L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT1U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT1L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT2U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT2L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT3U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_IBAT3L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT0U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT0L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT1U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT1L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT2U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT2L(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT3U(void);

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008 (Matching by size)
 */
u32 __TRK_get_DBAT3L(void);

/**
 * @TODO: Documentation
 */
DSError TRKValidMemory32(const void* addr, size_t length, ValidMemoryOptions readWriteable);

/**
 * @TODO: Documentation
 */
static ASM void TRK_ppc_memcpy(register void* dest, register const void* src, register int n, register u32 param_4, register u32 param_5) {
#ifdef __MWERKS__ // clang-format off
#define msr        r8
#define byte    r9
#define count    r10
	nofralloc

	mfmsr  msr
	li     count, 0

top_loop:
	cmpw   count, n
	beq    out_loop

	mtmsr  param_5
	sync

	lbzx   byte, count, src

	mtmsr  param_4
	sync

	stbx   byte, count, dest

	addi   count, count, 1

	b      top_loop
out_loop:
	mtmsr  msr
	sync

	blr
#undef count
#undef byte
#undef msr
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
DSError TRKTargetAccessMemory(void* data, u32 start, size_t* length, MemoryAccessOptions accessOptions, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKTargetReadInstruction(void* data, u32 start);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registerStorageSize, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKTargetVersions(DSVersions* versions);

/**
 * @TODO: Documentation
 */
DSError TRKTargetSupportMask(u8 mask[32]);

extern BOOL gTRKBigEndian;

/**
 * @TODO: Documentation
 */
DSError TRKTargetCPUType(DSCPUType* cpuType);


/**
 * @TODO: Documentation
 */
ASM void TRKInterruptHandler()
{
#ifdef __MWERKS__ // clang-format off
    nofralloc
    mtsrr0   r2
    mtsrr1   r4
    mfsprg   r4, 3
    mfcr     r2
    mtsprg   3, r2
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    lwz      r2, TRKState_PPC.MSR (r2)
    ori      r2, r2, MSR_EE | MSR_RI
    xori     r2, r2, MSR_EE | MSR_RI
    sync
    mtmsr    r2
    sync
    lis      r2,     TRK_saved_exceptionID @h
    ori      r2, r2, TRK_saved_exceptionID @l
    sth      r3, 0 (r2)
    cmpwi    r3, 0x500
    bne      L_802CF694
    lis      r2,     gTRKCPUState @h
    ori      r2, r2, gTRKCPUState @l
    mflr     r3
    stw      r3, ProcessorState_PPC.transport_handler_saved_ra (r2)
    bl       TRKUARTInterruptHandler
    lis      r2,     gTRKCPUState @h
    ori      r2, r2, gTRKCPUState @l
    lwz      r3, ProcessorState_PPC.transport_handler_saved_ra (r2)
    mtlr     r3
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    lwz      r2, TRKState_PPC.inputPendingPtr (r2)
    lbz      r2, TRKState_PPC.GPR[0] (r2)
    cmpwi    r2, 0
    beq      L_802CF678
    lis      r2,     gTRKExceptionStatus @h
    ori      r2, r2, gTRKExceptionStatus @l
    lbz      r2, TRKExceptionStatus.inTRK (r2)
    cmpwi    r2, 1
    beq      L_802CF678
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    li       r3, 1
    stb      r3, TRKState_PPC.inputActivated (r2)
    b        L_802CF694
L_802CF678:
    lis      r2,     gTRKSaveState @h
    ori      r2, r2, gTRKSaveState @l
    lwz      r3, Default_PPC.CR (r2)
    mtcrf    0xff, r3
    lwz      r3, Default_PPC.GPR[3] (r2)
    lwz      r2, Default_PPC.GPR[2] (r2)
    rfi
L_802CF694:
    lis      r2,     TRK_saved_exceptionID @h
    ori      r2, r2, TRK_saved_exceptionID @l
    lhz      r3, 0 (r2)
    lis      r2,     gTRKExceptionStatus @h
    ori      r2, r2, gTRKExceptionStatus @l
    lbz      r2, TRKExceptionStatus.inTRK (r2)
    cmpwi    r2, 0
    bne      TRKExceptionHandler
    lis      r2,     gTRKCPUState @h
    ori      r2, r2, gTRKCPUState @l
    stw      r0, ProcessorState_PPC.Default.GPR[0] (r2)
    stw      r1, ProcessorState_PPC.Default.GPR[1] (r2)
    mfsprg   r0, 1
    stw      r0, ProcessorState_PPC.Default.GPR[2] (r2)
    sth      r3, ProcessorState_PPC.Extended1.exceptionID (r2)
    sth      r3, (ProcessorState_PPC.Extended1.exceptionID + 2) (r2)
    mfsprg   r0, 2
    stw      r0, ProcessorState_PPC.Default.GPR[3] (r2)
    stmw     r4, ProcessorState_PPC.Default.GPR[4] (r2)
    mfsrr0   r27
    mflr     r28
    mfsprg   r29, 3
    mfctr    r30
    mfxer    r31
    stmw     r27, ProcessorState_PPC.Default.PC (r2)
    bl       TRKSaveExtended1Block
    lis      r2,     gTRKExceptionStatus @h
    ori      r2, r2, gTRKExceptionStatus @l
    li       r3, 1
    stb      r3, TRKExceptionStatus.inTRK (r2)
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    lwz      r0, TRKState_PPC.MSR (r2)
    sync
    mtmsr    r0
    sync
    lwz      r0, TRKState_PPC.LR (r2)
    mtlr     r0
    lwz      r0, TRKState_PPC.CTR (r2)
    mtctr    r0
    lwz      r0, TRKState_PPC.XER (r2)
    mtxer    r0
    lwz      r0, TRKState_PPC.DSISR (r2)
    mtdsisr  r0
    lwz      r0, TRKState_PPC.DAR (r2)
    mtdar    r0
    lmw      r3, TRKState_PPC.GPR[3] (r2)
    lwz      r0, TRKState_PPC.GPR[0] (r2)
    lwz      r1, TRKState_PPC.GPR[1] (r2)
    lwz      r2, TRKState_PPC.GPR[2] (r2)
    b        TRKPostInterruptEvent
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
ASM void TRKExceptionHandler(u16 a)
{
#ifdef __MWERKS__ // clang-format off
    nofralloc
    lis     r2,     gTRKExceptionStatus @h
    ori     r2, r2, gTRKExceptionStatus @l
    sth     r3, TRKExceptionStatus.exceptionInfo.exceptionID (r2)
    mfsrr0  r3
    stw     r3, TRKExceptionStatus.exceptionInfo.PC (r2)
    lhz     r3, TRKExceptionStatus.exceptionInfo.exceptionID (r2)
    cmpwi   r3, 0x200
    beq     LAB_00010ba4
    cmpwi   r3, 0x300
    beq     LAB_00010ba4
    cmpwi   r3, 0x400
    beq     LAB_00010ba4
    cmpwi   r3, 0x600
    beq     LAB_00010ba4
    cmpwi   r3, 0x700
    beq     LAB_00010ba4
    cmpwi   r3, 0x800
    beq     LAB_00010ba4
    cmpwi   r3, 0x1000
    beq     LAB_00010ba4
    cmpwi   r3, 0x1100
    beq     LAB_00010ba4
    cmpwi   r3, 0x1200
    beq     LAB_00010ba4
    cmpwi   r3, 0x1300
    beq     LAB_00010ba4
    b       LAB_00010bb0
LAB_00010ba4:
    mfsrr0  r3
    addi    r3, r3, 0x4
    mtsrr0  r3
LAB_00010bb0:
    lis     r2,     gTRKExceptionStatus @h
    ori     r2, r2, gTRKExceptionStatus @l
    li      r3, 0x1
    stb     r3, TRKExceptionStatus.exceptionDetected (r2)
    mfsprg  r3, 3
    mtcrf   0xff, r3
    mfsprg  r2, 1
    mfsprg  r3, 2
    rfi
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
void TRKPostInterruptEvent(void);

/**
 * @TODO: Documentation
 */
ASM void TRKSwapAndGo(void)
{
#ifdef __MWERKS__ // clang-format off
    nofralloc
    lis      r3,     gTRKState @h
    ori      r3, r3, gTRKState @l
    stmw     r0, TRKState_PPC.GPR[0] (r3)
    mfmsr    r0
    stw      r0, TRKState_PPC.MSR (r3)
    mflr     r0
    stw      r0, TRKState_PPC.LR (r3)
    mfctr    r0
    stw      r0, TRKState_PPC.CTR (r3)
    mfxer    r0
    stw      r0, TRKState_PPC.XER (r3)
    mfdsisr  r0
    stw      r0, TRKState_PPC.DSISR (r3)
    mfdar    r0
    stw      r0, TRKState_PPC.DAR (r3)
    li       r1, MSR_EE | MSR_RI
    nor      r1, r1, r1  // r1 = ~(MSR_EE | MSR_RI)
    mfmsr    r3
    and      r3, r3, r1
    mtmsr    r3
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    lwz      r2, TRKState_PPC.inputPendingPtr (r2)
    lbz      r2, TRKState_PPC.GPR[0] (r2)
    cmpwi    r2, 0
    beq      L_802CF930
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    li       r3, 1
    stb      r3, TRKState_PPC.inputActivated (r2)
    b        TRKInterruptHandlerEnableInterrupts
L_802CF930:
    lis      r2,     gTRKExceptionStatus @h
    ori      r2, r2, gTRKExceptionStatus @l
    li       r3, 0
    stb      r3, TRKExceptionStatus.inTRK (r2)
    bl       TRKRestoreExtended1Block
    lis      r2,     gTRKCPUState@h
    ori      r2, r2, gTRKCPUState@l
    lmw      r27, ProcessorState_PPC.Default.PC (r2)
    mtsrr0   r27
    mtlr     r28
    mtcrf    0xff, r29
    mtctr    r30
    mtxer    r31
    lmw      r3, ProcessorState_PPC.Default.GPR[3] (r2)
    lwz      r0, ProcessorState_PPC.Default.GPR[0] (r2)
    lwz      r1, ProcessorState_PPC.Default.GPR[1] (r2)
    lwz      r2, ProcessorState_PPC.Default.GPR[2] (r2)
    rfi
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
ASM void TRKInterruptHandlerEnableInterrupts(void)
{
#ifdef __MWERKS__ // clang-format off
    nofralloc;
    lis      r2,     gTRKState @h
    ori      r2, r2, gTRKState @l
    lwz      r0, TRKState_PPC.MSR (r2)
    sync
    mtmsr    r0
    sync
    lwz      r0, TRKState_PPC.LR (r2)
    mtlr     r0
    lwz      r0, TRKState_PPC.CTR (r2)
    mtctr    r0
    lwz      r0, TRKState_PPC.XER (r2)
    mtxer    r0
    lwz      r0, TRKState_PPC.DSISR (r2)
    mtdsisr  r0
    lwz      r0, TRKState_PPC.DAR (r2)
    mtdar    r0
    lmw      r3, TRKState_PPC.GPR[3] (r2)
    lwz      r0, TRKState_PPC.GPR[0] (r2)
    lwz      r1, TRKState_PPC.GPR[1] (r2)
    lwz      r2, TRKState_PPC.GPR[2] (r2)
    b        TRKPostInterruptEvent
#endif // clang-format on
}

/**
 * @TODO: Documentation
 */
DSError TRKTargetInterrupt(TRKEvent* event);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAddStopInfo(TRKBuffer* buffer);

/**
 * @TODO: Documentation
 */
DSError TRKTargetAddExceptionInfo(TRKBuffer* buffer);

/**
 * @TODO: Documentation
 */
static DSError TRKTargetEnableTrace(BOOL val);

/**
 * @TODO: Documentation
 */
static BOOL TRKTargetStepDone();

/**
 * @TODO: Documentation
 */
static DSError TRKTargetDoStep();

/**
 * @TODO: Documentation
 */
static BOOL TRKTargetCheckStep();

/**
 * @TODO: Documentation
 */
DSError TRKTargetSingleStep(u32 count, BOOL stepOver);

/**
 * @TODO: Documentation
 */
DSError TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver);

/**
 * @TODO: Documentation
 */
u32 TRKTargetGetPC();

/**
 * @TODO: Documentation
 */
DSError TRKTargetSupportRequest(void);

/**
 * @TODO: Documentation
 */
DSError TRKTargetFlushCache(u8 a, void* start, void* end);

/**
 * @TODO: Documentation
 */
BOOL TRKTargetStopped();

/**
 * @TODO: Documentation
 */
void TRKTargetSetStopped(uint stopped);

/**
 * @TODO: Documentation
 */
u32 TRKTargetStop();

/**
 * @TODO: Documentation
 */
DSError TRKPPCAccessSPR(void* value, u32 spr_register_num, BOOL read);

/**
 * @TODO: Documentation
 */
DSError TRKPPCAccessPairedSingleRegister(void* srcDestPtr, u32 psr, BOOL read);

#define FP_FPSCR_ACCESS 32
#define FP_FPECR_ACCESS 33

/**
 * @TODO: Documentation
 */
DSError TRKPPCAccessFPRegister(void* srcDestPtr, u32 fpr, BOOL read);

#define DEBUG_VECTORREG_ACCESS 0

/**
 * @TODO: Documentation
 */
DSError TRKPPCAccessSpecialReg(void* value, u32* access_func, BOOL read);

/**
 * @TODO: Documentation
 */
void TRKTargetSetInputPendingPtr(void* ptr)
{
	gTRKState.inputPendingPtr = ptr;
}

