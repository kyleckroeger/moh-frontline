// ARAM transfer callbacks (record the completion tick and clear the
// transfer-in-progress flag for the direction) and the animation-file
// queries: a state's info, callback id, transition and animation lists (the
// lists are appended to caller-provided arrays with a count and a capacity),
// an animation by index, and the file number and object type. The globals and
// AnimFileFormat_t are named by the symbols; their layouts and the table and
// list views are inferred. The rest of the file is not part of this unit.
extern "C" unsigned long OSGetTick(void);

extern unsigned long end;
extern bool g_bDMAToARAM;
extern bool g_bDMAToMRAM;

void DMAToARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToARAM = false;
}

void DMAFromARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToMRAM = false;
}

struct AnimFileAnimItem_t {
    unsigned short id;
    unsigned short pad;
    int data;
};

struct AnimFileAnimList_t {
    unsigned int count;
    AnimFileAnimItem_t items[1];
};

struct AnimFileState_t {
    unsigned short id;
    unsigned short pad;
    int callbackid;
    AnimFileAnimList_t* anims;
    void* info;
    void* transitions;
};

struct AnimFileStateTable_t {
    unsigned int count;
    AnimFileState_t states[1];
};

struct AnimFileFormat_t {
    unsigned char field00[8];
    unsigned short objecttype;
    unsigned short filenum;
    unsigned char field0c[4];
    AnimFileStateTable_t* states;
    void** animations;
};

struct AnimFileTransEntry_t {
    AnimFileFormat_t* file;
    unsigned short filenum;
    unsigned short pad;
    void* transitions;
};

struct AnimFileTransList_t {
    unsigned short count;
    unsigned short max;
    unsigned char field04[4];
    AnimFileTransEntry_t entries[1];
};

struct AnimFileAnimEntry_t {
    AnimFileFormat_t* file;
    unsigned short id;
    unsigned short pad;
    int data;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    unsigned short max;
    AnimFileAnimEntry_t entries[1];
};

extern "C" {

void* AnimFileGetStateInfo(AnimFileFormat_t* file, unsigned int id) {
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id)
            return file->states->states[i].info;
    }
    return 0;
}

void AnimFileGetStateTransList(AnimFileFormat_t* file, unsigned int id, AnimFileTransList_t* list) {
    if (list->count >= list->max)
        return;
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id) {
            void* transitions = file->states->states[i].transitions;
            if (!transitions)
                return;
            if (!*(unsigned int*)transitions)
                return;
            list->entries[list->count].file = file;
            list->entries[list->count].filenum = file->filenum;
            list->entries[list->count].transitions = file->states->states[i].transitions;
            list->count++;
            return;
        }
    }
}

void AnimFileGetStateAnimList(AnimFileFormat_t* file, unsigned int id, AnimFileStateAnimList_t* list) {
    if (list->count >= list->max)
        return;
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id) {
            for (unsigned int j = 0; j < file->states->states[i].anims->count; j++) {
                if (list->count >= list->max)
                    return;
                list->entries[list->count].file = file;
                list->entries[list->count].id = file->states->states[i].anims->items[j].id;
                list->entries[list->count].data = file->states->states[i].anims->items[j].data;
                list->count++;
            }
            return;
        }
    }
}

int AnimFileGetStateCallbackID(AnimFileFormat_t* file, unsigned int id) {
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id)
            return file->states->states[i].callbackid;
    }
    return -1;
}

void* AnimFileGetAnimation(AnimFileFormat_t* file, int index) {
    return file->animations[index + 1];
}

unsigned short AnimFileGetFileNum(AnimFileFormat_t* file) {
    return file->filenum;
}

unsigned short AnimFileGetObjectType(AnimFileFormat_t* file) {
    return file->objecttype;
}

}
