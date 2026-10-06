// SNDBANKI_valid: 0 when a bank slot holds a usable bank, -8 for a bad or
// empty slot, -18 while the bank is still loading. sndgs's layout is not
// known; the slot table (12-byte entries) is read through inferred offsets.
extern "C" char sndgs[];

struct SNDBANKSLOT {
    int* header;
    int field4;
    signed char loading;
};

int SNDBANKI_valid(int bank) {
    SNDBANKSLOT* slots;

    if (bank < 0 || bank >= *(unsigned short*)(sndgs + 40))
        return -8;
    slots = *(SNDBANKSLOT**)(sndgs + 472);
    if (!slots[bank].header)
        return -8;
    return slots[bank].loading ? -18 : 0;
}
