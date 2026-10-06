// Starts, updates and shuts down EA's REAL runtime (timers, threads, tasks,
// pad, file system). Return values of the callees are not used here.
extern "C" {
void ASYNCFILE_restore(void);
void FILESYS_restore(void);
void TIMER_restore(void);
void MEM_restore(void);
void REAL_restore(void);
void SYNCTASK_run(int);
unsigned int CPU_detect(void);
void TIMER_init(int);
void THREAD_init(void);
void SYNCTASK_init(void);
void PAD_init(void);
void PAD_update(void);
void SYNCTASK_del(void (*)(void));
void MEM_free(void*);
void FILESYS_setmemcallbacks(void* (*)(const char*, int, int), void (*)(void*));
void FILESYS_init(int, int, int);
void ASYNCFILE_init(int, int);
extern int TIMERhz;
}

void* DWI_alloc(const char*, int, int);

void ShutdownREAL() {
    ASYNCFILE_restore();
    FILESYS_restore();
    TIMER_restore();
    MEM_restore();
    REAL_restore();
}

void UpdateREAL() {
    SYNCTASK_run(0);
}

void InitREAL(int) {
    CPU_detect();
    TIMER_init(TIMERhz);
    THREAD_init();
    SYNCTASK_init();
    PAD_init();
    SYNCTASK_del(PAD_update);
    FILESYS_setmemcallbacks(DWI_alloc, MEM_free);
    FILESYS_init(0, 0, 0);
    ASYNCFILE_init(6, 0);
}
