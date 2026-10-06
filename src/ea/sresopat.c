// SNDBANKI_asyncresolvepatch: walk a patch's timbres and, for each one whose
// flags at offset 24 of the parsed header match the mask, resolve its sample
// asynchronously and run the user-data callback. The struct names come from
// the mangled symbols; the header's layout is not known, so the flag field is
// read through an inferred offset.
struct TAGGEDPATCH;

// The size of SNDIPATCHHEADER is not known; it only sets the stack frame.
struct SNDIPATCHHEADER {
    char data[196];
};

int SNDI_parsetimbre(void**, SNDIPATCHHEADER*);
void SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER*, char*, int*);
void SNDBANKI_userdatacallback(SNDIPATCHHEADER*, int);

int SNDBANKI_asyncresolvepatch(int mask, TAGGEDPATCH* patch, char* buffer, int* status) {
    void* p = patch;
    SNDIPATCHHEADER header;
    int more;

    p = (char*)p + 4;
    do {
        more = SNDI_parsetimbre(&p, &header);
        if (*(unsigned short*)(header.data + 24) & mask) {
            SNDPLATFORM_asyncresolvetimbre(&header, buffer, status);
            SNDBANKI_userdatacallback(&header, 0);
        }
    } while (more);
    return 0;
}
