#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/serpoll.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "trk.h"

static TRKFramingState gTRKFramingState;

void* gTRKInputPendingPtr;

extern int TRKPollUART(void);
MessageBufferID TRKTestForPacket(void);
void TRKProcessInput(int bufferIdx);

// TRKTestForPacket follows in the original file. Frontline's revision is
// an HDLC-style framing state machine (0x7E/0x7D, TRKReadUARTPoll) that
// is not reconstructed yet, so this unit covers only the functions before it.

void TRKGetInput(void) {
    TRKBuffer* msgbuffer;
    int bufID;
    u8 command;

    bufID = TRKTestForPacket();

    if (bufID != -1) {
        msgbuffer = TRKGetBuffer(bufID);
        TRKSetBufferPosition(msgbuffer, 0);
        TRKReadBuffer1_ui8(msgbuffer, &command);
        if (command < 0x80) {
            TRKProcessInput(bufID);
        } else {
            TRKReleaseBuffer(bufID);
        }
    }
}

void TRKProcessInput(int bufferIdx) {
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    event.msgBufID = bufferIdx;
    gTRKFramingState.msgBufID = -1;
    TRKPostEvent(&event);
}

DSError TRKInitializeSerialHandler() {
    gTRKFramingState.msgBufID = -1;
    gTRKFramingState.receiveState = DSRECV_Wait;
    gTRKFramingState.isEscape = FALSE;


    return DS_NoError;
}

DSError TRKTerminateSerialHandler(void) {
    return DS_NoError;
}
