// SNDbankremove: unload a bank (or every bank for -1): stop its voices, run
// the user-data callbacks for each timbre of each patch, and free its sample
// memory. sndgs's layout is not known; the slot and voice tables are accessed
// through inferred offsets. The struct names come from the mangled symbols.
extern "C" char sndgs[];

struct BANKVER5 {
    char field0[6];
    unsigned short patchcount;
};

struct SNDBANKSLOT {
    BANKVER5* header;
    unsigned int samples;
    unsigned char loading;
};

// The size of SNDIPATCHHEADER is not known; it only sets the stack frame,
// and sizes from 192 to 204 bytes give the original 240-byte frame.
struct SNDIPATCHHEADER {
    char data[200];
};

extern "C" {
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int SNDstop(int);
}
int SNDBANKI_valid(int);
void* SNDBANKI_getppatch(BANKVER5*, int);
int SNDI_parsetimbre(void**, SNDIPATCHHEADER*);
void SNDBANKI_userdatacallback(SNDIPATCHHEADER*, int);
void SNDPLATFORM_memfree(int, unsigned int);

extern "C" int SNDbankremove(int bank) {
    BANKVER5* header;
    int i;

    if (bank == -1) {
        for (i = 0; i < *(unsigned short*)(sndgs + 40); i++)
            SNDbankremove(i);
        return 0;
    }
    if (SNDBANKI_valid(bank) != 0)
        return -8;
    SNDSYS_entercritical();
    header = (*(SNDBANKSLOT**)(sndgs + 472))[bank].header;
    for (i = 0; i < *(short*)(sndgs + 368); i++) {
        char* entry = *(char**)(sndgs + 468) + i * 128;

        if (*(short*)(entry + 14) == bank)
            SNDstop(*(int*)entry);
    }
    for (i = 0; i < header->patchcount; i++) {
        void* patch = SNDBANKI_getppatch(header, i);

        if (patch) {
            SNDIPATCHHEADER patchheader;
            int more;

            patch = (char*)patch + 4;
            do {
                more = SNDI_parsetimbre(&patch, &patchheader);
                SNDBANKI_userdatacallback(&patchheader, 2);
            } while (more);
        }
    }
    if ((*(SNDBANKSLOT**)(sndgs + 472))[bank].samples)
        SNDPLATFORM_memfree(8, (*(SNDBANKSLOT**)(sndgs + 472))[bank].samples);
    (*(SNDBANKSLOT**)(sndgs + 472))[bank].header = 0;
    (*(SNDBANKSLOT**)(sndgs + 472))[bank].loading = 0;
    SNDSYS_leavecritical();
    return 0;
}
