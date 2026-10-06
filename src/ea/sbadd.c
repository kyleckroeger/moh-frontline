// SNDbankadd: register a bank whose header is in memory, download its sample
// data to sound memory in 4 KB pieces and resolve its patches; and the
// user-data callback run for each timbre of a patch. sndgs's layout is not
// known; the members used here are inferred views. The struct names come
// from the mangled symbols.
struct TAGGEDPATCH;

struct SNDGSVIEW {
    char field0[366];
    signed char userdataclients;
    char field16F[57];
    void (*userdataclient[7])(void*);
    int (*removebank)(int);
    char field1C8[16];
    struct SNDBANKSLOT* slots;
};

struct SNDBANKSLOT {
    struct BANKVER5* header;
    char* samples;
    unsigned char loading;
};

struct BANKVER5 {
    char field0[6];
    unsigned short patchcount;
    int sampleoffset;
    int samplesize;
    char field10[4];
    unsigned int patches[1];
};

struct SNDIPATCHHEADER {
    char field0[23];
    signed char userdatacount;
    char field18[28];
    int userdata[4];
    int userdatasize[4];
};

// Passed to the user-data clients. Only the first three members are set
// here; its size is not known (20 to 32 bytes give the original frame).
struct SNDUSERDATAINFO {
    int event;
    int data;
    int size;
    int field0C[2];
};

extern "C" {
extern SNDGSVIEW sndgs;
int SNDbankremove(int);
}
int SNDBANKI_alloc(void);
char* SNDPLATFORM_memalloc(int, int);
int SNDPLATFORM_download(int, void*, void*, int);
int SNDPLATFORM_downloadcomplete(int);
void SNDBANKI_asyncresolvepatch(int, TAGGEDPATCH*, char*, int*);

void SNDBANKI_userdatacallback(SNDIPATCHHEADER* header, int event) {
    while (header->userdatacount > 0) {
        SNDUSERDATAINFO info;
        int i;

        header->userdatacount--;
        info.event = event;
        info.data = header->userdata[header->userdatacount];
        info.size = header->userdatasize[header->userdatacount];
        for (i = 0; i < sndgs.userdataclients; i++)
            sndgs.userdataclient[i](&info);
    }
}

extern "C" int SNDbankadd(int* bank, void* data) {
    BANKVER5* header = (BANKVER5*)data;
    int offset;
    int resolved;
    int i;
    SNDBANKSLOT* slot;

    sndgs.removebank = SNDbankremove;
    *bank = SNDBANKI_alloc();
    slot = &sndgs.slots[*bank];
    slot->header = (BANKVER5*)data;
    slot->loading = 1;
    slot->samples = 0;
    if (header->samplesize) {
        slot->samples = SNDPLATFORM_memalloc(512, header->samplesize + 48);
        if (!slot->samples) {
            slot->header = 0;
            slot->loading = 0;
            return -6;
        }
        for (offset = 0; offset < header->samplesize; offset += 4096) {
            int size = header->samplesize - offset;
            int request;

            if (size > 4096)
                size = 4096;
            request = SNDPLATFORM_download(512, (char*)header + header->sampleoffset + offset, slot->samples + offset, size);
            while (!SNDPLATFORM_downloadcomplete(request))
                ;
        }
        resolved = i = 0;
        for (; i < header->patchcount; i++) {
            if (header->patches[i])
                SNDBANKI_asyncresolvepatch(512, (TAGGEDPATCH*)((char*)&header->patches[i] + header->patches[i]), slot->samples, &resolved);
        }
    }
    slot->loading = 0;
    if (header->samplesize)
        return 7;
    return 8;
}
