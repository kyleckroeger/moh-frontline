// A fragment of UIStudio.c (0x8008825c): UISSetScreenActive queues thread
// action 3 with a group record holding the two screen values (the rest of the
// record is left unset, no parameters) and runs the queued actions at once
// unless flag bit 1 of the studio record is set. UISInfo_t,
// UISThreadGroupInfoT, UISThreadActionT and UISParam_t are named by the
// mangled symbols; the views and the parameter meanings are inferred (see
// UISActionProcess.c). The rest of the file is not part of this unit.
struct UISParam_t;

enum UISThreadActionT { UISThreadActionUnknown = 0 };

union UISThreadGroupInfoT {
    struct {
        short s0;
        short s2;
        long l4;
        long l8;
        long l12;
    } s;
};

struct UISInfo_t {
    char field000[4];
    unsigned long flags;
};

void UISAddThreadAction(UISInfo_t* info, UISThreadActionT action, UISThreadGroupInfoT* group, long count,
                        UISParam_t* params);
void UISProcessThreadAction(UISInfo_t* info, unsigned char activation);

extern "C" void UISSetScreenActive(UISInfo_t* info, unsigned short screen, unsigned short control) {
    UISThreadGroupInfoT group;

    group.s.s0 = screen;
    group.s.s2 = control;
    UISAddThreadAction(info, (UISThreadActionT)3, &group, 0, 0);
    if (!(info->flags & 2))
        UISProcessThreadAction(info, 0);
}
