// UI thread actions: a downward-growing stack of queued actions (action id,
// a 16-byte group record, a parameter count and the parameters), run in
// order or, for the activation pass, only the control activations.
// UISInfo_t, UISParam_t, UISThreadGroupInfoT, UISThreadActionT and the
// called functions are named by the mangled symbols; the record views and
// field meanings are inferred. The file is compiled with deferred inlining
// (UISAddThreadAction, last in the image, is inlined into
// _UISDoThreadAction).
struct UISScreen_t;
struct UISControlInfo_t;
struct UISControlList_t;
struct UISStackInfo_t;

struct UISParam_t {
    unsigned long value;
};

// The action values are the case numbers of _UISDoThreadAction; their names
// are not known.
enum UISThreadActionT { UISThreadActionUnknown = 0 };

union UISThreadGroupInfoT {
    struct {
        unsigned short u0;
        unsigned short u2;
        unsigned short u4;
        unsigned short u6;
        long l8;
        unsigned short u12;
        unsigned short u14;
    } h;
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
    char field008[80];
    UISStackInfo_t* stack;
    char field05C[68];
    UISParam_t* actionBase;
    UISParam_t* actionTop;
};


void UISAddThreadAction(UISInfo_t* info, UISThreadActionT action, UISThreadGroupInfoT* group, long count,
                        UISParam_t* params) {
    UISParam_t* dst;
    UISParam_t* top = info->actionTop;

    top[0].value = action;
    *(UISThreadGroupInfoT*)(top - 5) = *group;
    top[-6].value = count;
    dst = top - 7;
    if (params) {
        for (long i = count - 1; i >= 0; i--)
            *dst-- = params[i];
    }
    info->actionTop = dst;
}

// _UISDoThreadAction (which inlines the function above) and
// UISProcessThreadAction precede this in the image; they are drafted in
// scratch but differ in register assignment, so this unit covers
// UISAddThreadAction only.
