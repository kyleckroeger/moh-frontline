// Animation states: each object carries four 104-byte state slots. This unit
// covers the slot accessors at the start of the file. The slot, owner and
// callback-table views are inferred from offsets; their names are not
// original.
struct AnimStateSlotView {
    unsigned char field00[4];
    unsigned short state;
    unsigned short status;
    unsigned short flags;
    unsigned char field0A[2];
    float delay;
    unsigned char userMemory[64];
    float weight;
    unsigned char field54[20];
};

struct AnimStateOwnerView {
    AnimStateSlotView slots[4];
};

struct AnimStCallbackEntryView {
    unsigned long id;
    void* callback;
};

struct AnimStCallbackTableView {
    unsigned long count;
    AnimStCallbackEntryView* entries;
};

extern AnimStCallbackTableView* _AnimSt_CallbackTable;

extern "C" void AnimStSetStateDelay(AnimStateOwnerView* owner, unsigned short state, float delay) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            slot->delay = delay;
    }
}

extern "C" void AnimStSetStateFlags(AnimStateOwnerView* owner, unsigned short state, unsigned short flags) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            slot->flags |= flags;
    }
}

extern "C" unsigned short AnimStGetCurrentState(AnimStateOwnerView* owner) {
    unsigned short current = 0xFFFF;

    for (int i = 3; i >= 0; i--) {
        if (owner->slots[i].status == 3)
            current = owner->slots[i].state;
    }
    return current;
}

extern "C" void* AnimStGetStateUserMemory(AnimStateOwnerView* owner, unsigned short state) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            return owner->slots[i].userMemory;
    }
    return 0;
}

extern "C" float AnimStGetStateWeight(AnimStateOwnerView* owner, unsigned short state) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            return owner->slots[i].weight;
    }
    return 0.0f;
}

extern "C" int AnimStGetCallbackIndexFromID(unsigned long id) {
    AnimStCallbackTableView* table = _AnimSt_CallbackTable;

    for (int i = 0; i < table->count; i++) {
        if (id == table->entries[i].id)
            return i;
    }
    return -1;
}
