// UI studio registration of the client's transform, resource and message
// callbacks in the studio record. The callback slots are inferred from their
// offsets in an inferred view of UISInfo_t, and the callback types are left
// generic. The UIS files are compiled with -inline deferred,auto, so the
// functions are listed in reverse image order. The rest of the file is not
// part of this unit.
typedef void (*UISCallback)(void);

struct UISInfo_t {
    unsigned char unknown00[12];
    UISCallback message;
    UISCallback resourceLoad;
    UISCallback resourceFree;
    UISCallback transform;
};

extern "C" {
void UISRegisterMessageFnc(UISInfo_t* info, UISCallback message) {
    info->message = message;
}

void UISRegisterResourceFncs(UISInfo_t* info, UISCallback load, UISCallback release) {
    info->resourceLoad = load;
    info->resourceFree = release;
}

void UISRegisterTransformFncs(UISInfo_t* info, UISCallback transform) {
    info->transform = transform;
}
}
