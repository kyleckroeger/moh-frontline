// SNDbankheadersize: a loaded bank's header size. sndgs is the sound
// system's global state; its layout is not known, so the fields used here are
// read through inferred offsets.
extern "C" char sndgs[];

struct SNDBANKSLOT {
    int* header;
    int field4;
    int field8;
};

extern "C" int SNDbankheadersize(int bank) {
    return (*(SNDBANKSLOT**)(sndgs + 472))[bank].header[2];
}
