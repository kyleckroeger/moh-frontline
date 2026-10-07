/* EA's AEMS asynchronous module-bank loader (saemsmbf.c).
   SNDAEMSI_asyncloadmodulebank records the request (file and stream names
   and offsets, load buffer, allocator), takes the first free bank slot,
   opens the file and registers SNDAEMSI_almbservice with the sound server;
   the service steps through the load: the 56-byte header read into the load
   buffer, the rest of the module data into memory from the allocator, the
   close, then the bank's patch and MIDI sound banks through SNDBANK_asyncload;
   when both are loaded it resolves the bank, records its sound bank handles,
   installs it and frees the names. SNDAEMS_asyncloadmodulebankdone reports
   whether a load is pending. The global names come from the symbols;
   MODULEBANK is named by the mangled symbols, its members (and sndaems's)
   are inferred. */
struct AEMSBANKINFOVIEW {
    int soundBank;
    int soundBank2;
};

struct MODULEBANK {
    unsigned char unknown00[4];
    int size;
    unsigned char unknown08[20];
    unsigned int patchOffset;
    int moduleSize;
    unsigned int midiOffset;
    int soundBankSize;
    unsigned char unknown2c[4];
    AEMSBANKINFOVIEW* info;
};

struct SNDAEMSVIEW {
    unsigned char unknown00[12];
    MODULEBANK** banks;
    unsigned char unknown10[24];
    unsigned char loading;
    unsigned char unknown29[27];
};

extern SNDAEMSVIEW sndaems;

extern "C" {
int FILESYS_opstatus(int);
int FILESYS_completeop(int);
int FILESYS_read(int, int, void*, int, int, void*);
int FILESYS_close(int, int, void*);
int FILESYS_open(const char*, int, int, void*);
int SNDBANK_asyncload(char*, int, void*, int, void* (*)(int));
int SNDBANK_asyncdone();
void SNDI_memcpy(void*, const void*, int);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
unsigned long strlen(const char*);
char* strcpy(char*, const char*);
}
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
void iSNDserveraddclient(void (*)());
void iSNDserverremoveclient(void (*)());
void SNDAEMSI_resolvemodulebank(MODULEBANK*, char*, int);
int SNDAEMSI_asyncloadmodulebank(char*, int, char*, int, void*, int, void* (*)(int));

int SNDAEMSalmbcurrentpriority = 100;
int (*SNDAEMS_asyncloadmodulebank)(char*, int, char*, int, void*, int, void* (*)(int)) = SNDAEMSI_asyncloadmodulebank;

int SNDAEMSalmbpriority;
char* SNDAEMSalmbfilename;
int SNDAEMSalmbfileoffset;
char* SNDAEMSalmbstreamfilename;
int SNDAEMSalmbstreamfileoffset;
void* SNDAEMSalmbploadbuf;
int SNDAEMSalmbloadbufsize;
void* (*SNDAEMSalmbmalloccb)(int);
int SNDAEMSalmbopenop;
int SNDAEMSalmbcloseop;
int SNDAEMSalmbfirstreadop;
int SNDAEMSalmbsecondreadop;
int SNDAEMSalmbfhandle;
MODULEBANK* SNDAEMSalmbpheader;
MODULEBANK* SNDAEMSalmbpmb;
int SNDAEMSalmbmodulebankhandle;
int SNDAEMSalmbpatchbankhandle;
int SNDAEMSalmbmidibankhandle;
char SNDAEMSalmbsfxloaded;
char SNDAEMSalmbmidiloaded;

void SNDAEMSI_almbservice() {
    if (SNDAEMSalmbopenop) {
        if (FILESYS_opstatus(SNDAEMSalmbopenop) == 1) {
            SNDAEMSalmbfhandle = FILESYS_completeop(SNDAEMSalmbopenop);
            SNDAEMSalmbopenop = 0;
            SNDAEMSalmbfirstreadop = FILESYS_read(SNDAEMSalmbfhandle, SNDAEMSalmbfileoffset, SNDAEMSalmbploadbuf, 56,
                                                  SNDAEMSalmbpriority, 0);
        } else {
            return;
        }
    }
    if (SNDAEMSalmbfirstreadop && FILESYS_opstatus(SNDAEMSalmbfirstreadop) == 1) {
        int size;

        FILESYS_completeop(SNDAEMSalmbfirstreadop);
        SNDAEMSalmbfirstreadop = 0;
        SNDAEMSalmbpmb = (MODULEBANK*)SNDAEMSalmbploadbuf;
        size = SNDAEMSalmbpmb->size - (SNDAEMSalmbpmb->moduleSize + SNDAEMSalmbpmb->soundBankSize);
        SNDAEMSalmbpheader = (MODULEBANK*)SNDAEMSalmbmalloccb(size);
        SNDI_memcpy(SNDAEMSalmbpheader, SNDAEMSalmbploadbuf, 56);
        SNDAEMSalmbpmb = SNDAEMSalmbpheader;
        SNDAEMSalmbsecondreadop = FILESYS_read(SNDAEMSalmbfhandle, SNDAEMSalmbfileoffset + 56,
                                               (char*)SNDAEMSalmbpheader + 56, size - 56, SNDAEMSalmbpriority, 0);
    }
    if (SNDAEMSalmbsecondreadop && FILESYS_opstatus(SNDAEMSalmbsecondreadop) == 1) {
        FILESYS_completeop(SNDAEMSalmbsecondreadop);
        SNDAEMSalmbcloseop = FILESYS_close(SNDAEMSalmbfhandle, SNDAEMSalmbpriority, 0);
    }
    if (SNDAEMSalmbcloseop && FILESYS_opstatus(SNDAEMSalmbcloseop) == 1) {
        FILESYS_completeop(SNDAEMSalmbcloseop);
        if (SNDAEMSalmbpmb->patchOffset)
            SNDAEMSalmbpatchbankhandle =
                SNDBANK_asyncload(SNDAEMSalmbfilename, SNDAEMSalmbfileoffset + SNDAEMSalmbpmb->patchOffset,
                                  SNDAEMSalmbploadbuf, SNDAEMSalmbloadbufsize, SNDAEMSalmbmalloccb);
        else
            SNDAEMSalmbsfxloaded = 1;
    }
    if (SNDAEMSalmbpmb) {
        if (SNDAEMSalmbpatchbankhandle >= 0 && !SNDAEMSalmbsfxloaded) {
            if (SNDBANK_asyncdone() > 0)
                SNDAEMSalmbsfxloaded = 1;
            else
                return;
        }
        if (SNDAEMSalmbsfxloaded) {
            if (SNDAEMSalmbmidibankhandle < 0) {
                if (SNDAEMSalmbpmb->midiOffset)
                    SNDAEMSalmbmidibankhandle =
                        SNDBANK_asyncload(SNDAEMSalmbfilename, SNDAEMSalmbfileoffset + SNDAEMSalmbpmb->midiOffset,
                                          SNDAEMSalmbploadbuf, SNDAEMSalmbloadbufsize, SNDAEMSalmbmalloccb);
                else
                    SNDAEMSalmbmidiloaded = 1;
            }
            if (!SNDAEMSalmbmidiloaded && SNDBANK_asyncdone() > 0)
                SNDAEMSalmbmidiloaded = 1;
        }
    }
    if (SNDAEMSalmbmidiloaded) {
        iSNDserverremoveclient(SNDAEMSI_almbservice);
        SNDSYS_entercritical();
        SNDAEMSI_resolvemodulebank(SNDAEMSalmbpmb, SNDAEMSalmbstreamfilename, SNDAEMSalmbstreamfileoffset);
        SNDAEMSalmbpmb->info->soundBank = SNDAEMSalmbpatchbankhandle;
        SNDAEMSalmbpmb->info->soundBank2 = SNDAEMSalmbmidibankhandle;
        sndaems.banks[SNDAEMSalmbmodulebankhandle] = SNDAEMSalmbpmb;
        SNDMEMI_free(SNDAEMSalmbfilename);
        if (SNDAEMSalmbstreamfilename)
            SNDMEMI_free(SNDAEMSalmbstreamfilename);
        sndaems.loading = 0;
        SNDSYS_leavecritical();
    }
}

int SNDAEMSI_asyncloadmodulebank(char* filename, int fileoffset, char* streamfilename, int streamfileoffset, void* buffer,
                                 int buffersize, void* (*malloccb)(int)) {
    SNDAEMSalmbploadbuf = buffer;
    sndaems.loading = 1;
    SNDAEMSalmbloadbufsize = buffersize;
    SNDAEMSalmbmalloccb = malloccb;
    SNDSYS_entercritical();
    SNDAEMSalmbfilename = (char*)SNDMEMI_allocz(strlen(filename) + 1);
    strcpy(SNDAEMSalmbfilename, filename);
    SNDAEMSalmbfileoffset = fileoffset;
    if (streamfilename && *(unsigned char*)streamfilename) {
        SNDAEMSalmbstreamfilename = (char*)SNDMEMI_allocz(strlen(streamfilename) + 1);
        strcpy(SNDAEMSalmbstreamfilename, streamfilename);
    } else {
        SNDAEMSalmbstreamfilename = 0;
    }
    SNDAEMSalmbstreamfileoffset = streamfileoffset;
    SNDSYS_leavecritical();
    SNDAEMSalmbfirstreadop = 0;
    SNDAEMSalmbsecondreadop = 0;
    SNDAEMSalmbsfxloaded = 0;
    SNDAEMSalmbmidiloaded = 0;
    SNDAEMSalmbpatchbankhandle = -1;
    SNDAEMSalmbmidibankhandle = -1;
    SNDAEMSalmbpriority = SNDAEMSalmbcurrentpriority;
    for (SNDAEMSalmbmodulebankhandle = 0; SNDAEMSalmbmodulebankhandle < 16; SNDAEMSalmbmodulebankhandle++) {
        if (!sndaems.banks[SNDAEMSalmbmodulebankhandle])
            break;
    }
    SNDAEMSalmbopenop = FILESYS_open(filename, 1, SNDAEMSalmbpriority, 0);
    iSNDserveraddclient(SNDAEMSI_almbservice);
    SNDAEMSI_almbservice();
    return SNDAEMSalmbmodulebankhandle;
}

extern "C" unsigned char SNDAEMS_asyncloadmodulebankdone() {
    return sndaems.loading == 0;
}
