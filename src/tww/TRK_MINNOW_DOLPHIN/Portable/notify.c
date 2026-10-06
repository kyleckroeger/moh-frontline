#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/notify.h"
#include "trk.h"

// Frontline's revision appends the command byte to the message, and the
// append is inlined. Where the original defines this inline copy of
// msgbuf.c's TRKAppendBuffer1_ui8 is not known.
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}

DSError TRKDoNotifyStopped(u8 cmd) {
    DSError err;
    int reqIdx;
    int bufIdx;
    TRKBuffer* msg;

    err = TRKGetFreeBuffer(&bufIdx, &msg);
    if (err == DS_NoError) {
        err = TRKAppendBuffer1_ui8(msg, cmd);

        if (err == DS_NoError) {
            if (cmd == DSMSG_NotifyStopped) {
                TRKTargetAddStopInfo(msg);
            } else {
                TRKTargetAddExceptionInfo(msg);
            }
        }

        err = TRKRequestSend(msg, &reqIdx, 2, 3, 1);
        if (err == DS_NoError) {
            TRKReleaseBuffer(reqIdx);
        }
        TRKReleaseBuffer(bufIdx);
    }

    return err;
}
