/* A fragment of Frontline's targimpl.c (0x80135d8c): stopping the target,
   setting and reading the stopped flag, and flushing the cache over a range.
   The bodies are Pikmin's CC0 targimpl.c (https://github.com/doldecomp/pikmin),
   compiled like the other targimpl.c fragments (targimpl.c, targimpl_head.c)
   at GC 1.3. The file name is this project's; the original record is
   targimpl.c. The other functions are declared only and the file's data is
   declared extern (its file-local objects without static). */
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

u32 __TRK_get_MSR();

/**
 * @TODO: Documentation
 */
void __TRK_set_MSR(register u32 msr);

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
static void TRK_ppc_memcpy(register void* dest, register const void* src, register int n, register u32 param_4, register u32 param_5);

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
void TRKInterruptHandler();

/**
 * @TODO: Documentation
 */
void TRKExceptionHandler(u16 a);

/**
 * @TODO: Documentation
 */
void TRKPostInterruptEvent(void);

/**
 * @TODO: Documentation
 */
void TRKSwapAndGo(void);

/**
 * @TODO: Documentation
 */
void TRKInterruptHandlerEnableInterrupts(void);

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
DSError TRKTargetFlushCache(u8 a, void* start, void* end)
{
	if (start < end) {
		TRK_flush_cache(start, (u8*)end - (u8*)start);
		return DS_NoError;
	}

	return DS_InvalidMemory;
}

/**
 * @TODO: Documentation
 */
BOOL TRKTargetStopped()
{
	return gTRKState.isStopped;
}

/**
 * @TODO: Documentation
 */
void TRKTargetSetStopped(uint stopped)
{
	gTRKState.isStopped = stopped;
}

/**
 * @TODO: Documentation
 */
u32 TRKTargetStop()
{
	TRKTargetSetStopped(TRUE);
	return 0;
}

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
void TRKTargetSetInputPendingPtr(void* ptr);

