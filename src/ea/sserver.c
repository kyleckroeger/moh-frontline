// Sound server critical sections: the sound mutex with a nesting count in
// sndgs (inferred offset).
extern "C" char sndgs[];

void SNDI_mutexlock(void);
void SNDI_mutexunlock(void);

// SNDSYSI_100hzserver comes first in the original file and is not part of
// this unit.

extern "C" void SNDSYS_entercritical(void) {
    SNDI_mutexlock();
    sndgs[363]++;
}

extern "C" void SNDSYS_leavecritical(void) {
    sndgs[363]--;
    SNDI_mutexunlock();
}
