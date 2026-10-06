/* The sound driver's mutex wrappers, the last functions of the file: they
   initialise, lock and unlock the driver's mutex through the Dolphin OS
   (freeing it does nothing). snddrv is named by its symbol; the view of it is
   inferred (only the mutex's offset is known; the rest of the 0xab20-byte
   structure is opaque). */
struct OSMutex {
    unsigned char unknown00[24];
};

extern "C" {
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
}

struct SNDDRVVIEW {
    unsigned char unknown0000[0xa8a8];
    OSMutex mutex;
    unsigned char unknowna8c0[0x260];
};

extern SNDDRVVIEW snddrv;

void SNDI_mutexalloc() {
    OSInitMutex(&snddrv.mutex);
}

void SNDI_mutexfree() {
}

void SNDI_mutexlock() {
    OSLockMutex(&snddrv.mutex);
}

void SNDI_mutexunlock() {
    OSUnlockMutex(&snddrv.mutex);
}
