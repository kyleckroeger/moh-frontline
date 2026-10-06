// The mixer's allocation hooks: SNDI_New takes cleared sound memory and
// SNDI_Delete frees it. The rest of the file is not part of this unit.
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

void* SNDI_New(unsigned long size) {
    return SNDMEMI_allocz(size);
}

void SNDI_Delete(void* memory) {
    SNDMEMI_free(memory);
}
