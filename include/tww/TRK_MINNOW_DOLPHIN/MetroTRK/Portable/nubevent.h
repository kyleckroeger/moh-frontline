#ifndef METROTRK_PORTABLE_NUBEVENT_H
#define METROTRK_PORTABLE_NUBEVENT_H

#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 NubEventID;

// The event type is a byte, as in Pikmin's CC0 trktypes.h
// (https://github.com/doldecomp/pikmin): targimpl.c, compiled with int-sized
// enums, reads it with lbz.
typedef struct TRKEvent {
    u8 eventType;
    NubEventID eventID;
    MessageBufferID msgBufID;
} TRKEvent;

typedef struct TRKEventQueue  {
    int _00;
    int count;
    int next;
    TRKEvent events[2];
    NubEventID eventID;
} TRKEventQueue;
extern TRKEventQueue gTRKEventQueue;

BOOL TRKGetNextEvent(TRKEvent* event);
void TRKDestructEvent(TRKEvent*);
void TRKConstructEvent(TRKEvent*, NubEventType);
DSError TRKPostEvent(TRKEvent*);
DSError TRKInitializeEventQueue();

#ifdef __cplusplus
}
#endif

#endif /* METROTRK_PORTABLE_NUBEVENT_H */
