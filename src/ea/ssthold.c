// SNDSTRM_modifyhold: change a queued stream request's hold value. The
// request's layout is not known; it is accessed through an inferred offset.
extern "C" {
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
char* SNDSTRMI_getrequestptr(int);

extern "C" int SNDSTRM_modifyhold(int request, short hold) {
    int result = -8;
    char* entry;

    SNDSYS_entercritical();
    entry = SNDSTRMI_getrequestptr(request);
    if (entry) {
        *(int*)(entry + 32) = hold;
        result = 0;
    }
    SNDSYS_leavecritical();
    return result;
}
