/* A fragment of Frontline's dolphin_trk_glue.c (0x80137884): EnableEXI2Interrupts
   and TRKInitializeIntDrivenUART, which call the debugger communication
   table's interrupt set-up and initialisation (with TRKEXICallBack as the
   handler). The file name is this project's; the original record is
   dolphin_trk_glue.c (dolphin_trk_glue.c holds the start of the file) and the
   functions around these are not reconstructed. Structure as in the TWW
   dolphin_trk_glue.c this unit's sibling follows; the file's data is extern. */
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"
#include "dolphin/db/db.h"
#include "dolphin/amcstubs/AmcExi2Stubs.h"
#include "trk.h"

// Frontline links the older MetroTRK glue that Pikmin's decompilation
// (doldecomp/pikmin, CC0) reconstructs: a seven-entry DBCommTable for the
// GDEV debugger or the AMC EXI2 stubs, and buffered UART reads and writes.
// Asm functions are emitted first; the C functions are emitted in reverse
// source order (deferred inlining), so TRKUARTInterruptHandler, the second
// function in the image, is defined last.

#define BUFF_LEN 4362

BOOL Hu_IsStub(void);
void TRKInterruptHandler();

extern u8 gWriteBuf[BUFF_LEN];
extern u8 gReadBuf[BUFF_LEN];
extern BOOL _MetroTRK_Has_Framing;
extern s32 gReadCount;
extern s32 gReadPos;
extern s32 gWritePos;

extern DBCommTable gDBCommTable;

asm void TRKLoadContext(OSContext* ctx, u32);

void TRKEXICallBack(s16 param_0, OSContext* ctx);

int InitMetroTRKCommTable(int hwId);

DSError TRKInitializeIntDrivenUART(u32 param_0, u32 param_1, u32 param_2, void* param_3) {
    gDBCommTable.initialize_func(param_3, TRKEXICallBack);
    return DS_NoError;
}

void EnableEXI2Interrupts(void) {
    gDBCommTable.init_interrupts_func();
}

int TRKPollUART(void);

UARTError TRKReadUARTN(void* bytes, u32 length);

UARTError TRKWriteUARTN(const void* bytes, u32 length);

UARTError WriteUARTFlush(void);

UARTError WriteUART1(u8 arg0);

UARTError TRKReadUARTPoll(u8* arg0);

void ReserveEXI2Port(void);

void UnreserveEXI2Port(void);

void TRK_board_display(char* str);

void TRKUARTInterruptHandler();


