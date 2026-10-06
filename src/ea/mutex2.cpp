// REAL mutexes on OS mutexes; destroying one poisons its first word. The
// record's layout is inferred from offsets.
struct OSMutex {
    int data[6];
};

struct MUTEX {
    int field0;
    OSMutex mutex;
};

extern "C" {
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
void MEM_fill(void*, int, int);

int MUTEX_create(MUTEX* mutex) {
    OSInitMutex(&mutex->mutex);
    return 1;
}

void MUTEX_destroy(MUTEX* mutex) {
    MEM_fill(mutex, 0xdeadbeef, 4);
}

void MUTEX_lock(MUTEX* mutex) {
    OSLockMutex(&mutex->mutex);
}

void MUTEX_unlock(MUTEX* mutex) {
    OSUnlockMutex(&mutex->mutex);
}
}
