/* Frontline's target implementation, the functions at the end of the file
   record: register-block and memory access and the memory-range check. The
   bodies are Pikmin's CC0 targimpl.c (https://github.com/doldecomp/pikmin),
   compiled with the TWW headers plus Pikmin's DSVersions/DSCPUType types and
   MSR bits (Dolphin/PPCArch.h), at GC 1.3 like mem_TRK.c. The other
   functions of the file are declared only: the head (MSR access, memcpy,
   interrupt and exception handlers) matches but cannot share a unit with this
   tail, and the middle (FP register access, support requests, stop
   information, the interrupt check, extended-2 access) differs; the full
   Pikmin-based draft is scratch/lib/targimpl_pik.c. The CPU and TRK state,
   the save state and the 128-bit temporary are declared extern: they follow
   TRK_saved_exceptionID in .bss and are left with the rest of the file.
   TRK_ppc_memcpy, a file-local function earlier in the record, is declared
   without static so the unit can refer to it. */
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

const memRange gTRKMemMap[1] = { { (u8*)0, (u8*)-1, TRUE, TRUE } };

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

ProcessorRestoreFlags_PPC gTRKRestoreFlags    = { FALSE, FALSE };
static TRKExceptionStatus gTRKExceptionStatus = { { 0, 0, 0 }, TRUE, 0 };
static TRKStepStatus gTRKStepStatus           = { FALSE, DSSTEP_IntoCount, 0, 0 };
static u16 TRK_saved_exceptionID              = 0;
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


void __TRK_set_MSR(register u32 msr);


u32 __TRK_get_PVR(void);

u32 __TRK_get_IBAT0U(void);

u32 __TRK_get_IBAT0L(void);

u32 __TRK_get_IBAT1U(void);

u32 __TRK_get_IBAT1L(void);

u32 __TRK_get_IBAT2U(void);

u32 __TRK_get_IBAT2L(void);

u32 __TRK_get_IBAT3U(void);

u32 __TRK_get_IBAT3L(void);

u32 __TRK_get_DBAT0U(void);

u32 __TRK_get_DBAT0L(void);

u32 __TRK_get_DBAT1U(void);

u32 __TRK_get_DBAT1L(void);

u32 __TRK_get_DBAT2U(void);

u32 __TRK_get_DBAT2L(void);

u32 __TRK_get_DBAT3U(void);

u32 __TRK_get_DBAT3L(void);

DSError TRKValidMemory32(const void* addr, size_t length, ValidMemoryOptions readWriteable)
{
	DSError err = DS_InvalidMemory; /* assume range is invalid */

	const u8* start;
	const u8* end;

	int i;

	/*
	** Get start and end addresses for the memory range and
	** verify that they are reasonable.
	*/

	start = (const u8*)addr;
	end   = ((const u8*)addr + (length - 1));

	if (end < start)
		return DS_InvalidMemory;

	/*
	** Iterate through the gTRKMemMap array to determine if the requested
	** range falls within the valid ranges in the map.
	*/

	for (i = 0; (i < (s32)(sizeof(gTRKMemMap) / sizeof(memRange))); i++) {
		/*
		** If the requested range is not completely above
		** the valid range AND it is not completely below
		** the valid range then it must overlap somewhere.
		** If the requested range overlaps with one of the
		** valid ranges, do some additional checking.
		**
		*/

		if ((start <= (const u8*)gTRKMemMap[i].end) && (end >= (const u8*)gTRKMemMap[i].start)) {
			/*
			** First, verify that the read/write attributes are
			** acceptable.  If so, then recursively check any
			** part of the requested range that falls before or
			** after the valid range.
			*/

			if ((((u8)readWriteable == VALIDMEM_Readable) && !gTRKMemMap[i].readable)
			    || (((u8)readWriteable == VALIDMEM_Writeable) && !gTRKMemMap[i].writeable)) {
				err = DS_InvalidMemory;
			} else {
				err = DS_NoError;

				/*
				** If a portion of the requested range falls before
				** the current valid range, then recursively
				** check it.
				*/

				if (start < (const u8*)gTRKMemMap[i].start)
					err = TRKValidMemory32(start, (u32)((const u8*)gTRKMemMap[i].start - start), readWriteable);

				/*
				** If a portion of the requested range falls after
				** the current valid range, then recursively
				** check it.
				** Note: Only do this step if the previous check
				** did not detect invalid access.
				*/

				if ((err == DS_NoError) && (end > (const u8*)gTRKMemMap[i].end))
					err = TRKValidMemory32((const u8*)gTRKMemMap[i].end, (u32)(end - (const u8*)gTRKMemMap[i].end), readWriteable);
			}

			break;
		}
	}

	return err;
}

void TRK_ppc_memcpy(register void* dest, register const void* src, register int n, register u32 param_4, register u32 param_5);


DSError TRKTargetAccessMemory(void* data, u32 start, size_t* length, MemoryAccessOptions accessOptions, BOOL read)
{
	DSError error;
	u32 target_msr;
	void* addr;
	u32 trk_msr;
	TRKExceptionStatus tempExceptionStatus = gTRKExceptionStatus;
	gTRKExceptionStatus.exceptionDetected  = FALSE;

	addr  = (void*)TRKTargetTranslate(start);
	error = TRKValidMemory32(addr, *length, read ? VALIDMEM_Readable : VALIDMEM_Writeable);

	if (error != DS_NoError) {
		*length = 0;
	} else {
		target_msr = __TRK_get_MSR();
		trk_msr    = target_msr | gTRKCPUState.Extended1.MSR & MSR_DR;

		if (read) {
			TRK_ppc_memcpy(data, addr, *length, target_msr, trk_msr);
		} else {
			TRK_ppc_memcpy(addr, data, *length, trk_msr, target_msr);
			TRK_flush_cache(addr, *length);
			if ((void*)start != addr) {
				TRK_flush_cache((void*)start, *length);
			}
		}
	}

	if (gTRKExceptionStatus.exceptionDetected) {
		*length = 0;
		error   = DS_CWDSException;
	}

	gTRKExceptionStatus = tempExceptionStatus;
	return error;
}

DSError TRKTargetReadInstruction(void* data, u32 start);

DSError TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read)
{
	DSError error;
	u32 count;
	u32* data;
	TRKExceptionStatus tempExceptionStatus;

	if (lastRegister > 0x24) {
		return DS_InvalidRegister;
	}

	tempExceptionStatus = gTRKExceptionStatus;

	gTRKExceptionStatus.exceptionDetected = FALSE;

	data = gTRKCPUState.Default.GPR + firstRegister;

	count = (lastRegister - firstRegister) + 1;

	*registersLengthPtr = count * sizeof(u32);

	if (read) {
		error = TRKAppendBuffer_ui32(b, data, count);
	} else {
		error = TRKReadBuffer_ui32(b, data, count);
	}

	if (gTRKExceptionStatus.exceptionDetected) {
		*registersLengthPtr = 0;
		error               = DS_CWDSException;
	}

	gTRKExceptionStatus = tempExceptionStatus;
	return error;
}

DSError TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read)
{
	u64 temp;
	DSError error;
	TRKExceptionStatus tempExceptionStatus;
	u32 current;

	if (lastRegister > 0x21) {
		return DS_InvalidRegister;
	}

	tempExceptionStatus                   = gTRKExceptionStatus;
	gTRKExceptionStatus.exceptionDetected = FALSE;

	__TRK_set_MSR(__TRK_get_MSR() | 0x2000);

	*registersLengthPtr = 0;
	error               = DS_NoError;

	for (current = firstRegister; (current <= lastRegister) && (error == DS_NoError); current++, *registersLengthPtr += sizeof(f64)) {
		if (read) {
			TRKPPCAccessFPRegister(&temp, current, read);
			error = TRKAppendBuffer1_ui64(b, temp);
		} else {
			TRKReadBuffer1_ui64(b, &temp);
			error = TRKPPCAccessFPRegister(&temp, current, read);
		}
	}

	if (gTRKExceptionStatus.exceptionDetected) {
		*registersLengthPtr = 0;
		error               = DS_CWDSException;
	}

	gTRKExceptionStatus = tempExceptionStatus;
	return error;
}

DSError TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read)
{
	TRKExceptionStatus tempExceptionStatus;
	int error;
	u32* data;
	int count;

	if (lastRegister > 0x60) {
		return DS_InvalidRegister;
	}

	tempExceptionStatus                   = gTRKExceptionStatus;
	gTRKExceptionStatus.exceptionDetected = FALSE;

	*registersLengthPtr = 0;

	if (firstRegister <= lastRegister) {
		data  = (u32*)&gTRKCPUState.Extended1 + firstRegister;
		count = lastRegister - firstRegister + 1;
		*registersLengthPtr += count * sizeof(u32);

		if (read) {
			error = TRKAppendBuffer_ui32(b, data, count);
		} else {
			if (data <= &gTRKCPUState.Extended1.TBU && (data + count - 1) >= &gTRKCPUState.Extended1.TBL) {
				gTRKRestoreFlags.TBR = 1;
			}

			if (data <= &gTRKCPUState.Extended1.DEC && (data + count - 1) >= &gTRKCPUState.Extended1.DEC) {
				gTRKRestoreFlags.DEC = 1;
			}
			error = TRKReadBuffer_ui32(b, data, count);
		}
	}
	if (gTRKExceptionStatus.exceptionDetected) {
		*registersLengthPtr = 0;
		error               = DS_CWDSException;
	}

	gTRKExceptionStatus = tempExceptionStatus;
	return error;
}

DSError TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registerStorageSize, BOOL read);


DSError TRKTargetVersions(DSVersions* versions);


DSError TRKTargetSupportMask(u8 mask[32]);


extern BOOL gTRKBigEndian;

DSError TRKTargetCPUType(DSCPUType* cpuType);



void TRKInterruptHandler();


void TRKExceptionHandler(u16 a);


void TRKPostInterruptEvent(void);


void TRKSwapAndGo(void);


void TRKInterruptHandlerEnableInterrupts(void);


DSError TRKTargetInterrupt(TRKEvent* event);


DSError TRKTargetAddStopInfo(TRKBuffer* buffer);


DSError TRKTargetAddExceptionInfo(TRKBuffer* buffer);










DSError TRKTargetSingleStep(u32 count, BOOL stepOver);


DSError TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver);


u32 TRKTargetGetPC();


DSError TRKTargetSupportRequest(void);


DSError TRKTargetFlushCache(u8 a, void* start, void* end);


BOOL TRKTargetStopped();


void TRKTargetSetStopped(uint stopped);


u32 TRKTargetStop();


DSError TRKPPCAccessSPR(void* value, u32 spr_register_num, BOOL read);

DSError TRKPPCAccessPairedSingleRegister(void* srcDestPtr, u32 psr, BOOL read);

#define FP_FPSCR_ACCESS 32
#define FP_FPECR_ACCESS 33

DSError TRKPPCAccessFPRegister(void* srcDestPtr, u32 fpr, BOOL read);


#define DEBUG_VECTORREG_ACCESS 0

DSError TRKPPCAccessSpecialReg(void* value, u32* access_func, BOOL read);

void TRKTargetSetInputPendingPtr(void* ptr);

