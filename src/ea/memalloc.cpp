// The allocation entry points at the end of memalloc.cpp: MEM_allocalign,
// MEM_alloc and MEM_allocz forward to MEM_allocaligna, the latter two with no
// alignment; the final flag is true except in MEM_allocz (its meaning is not
// established). The parameter names are inferred; MEM_allocaligna itself and
// the rest of the file are not part of this unit.
void* MEM_allocaligna(const char*, int, int, int, int, bool);

extern "C" {
void* MEM_allocalign(const char* name, int size, int align, int offset, int flags) {
    return MEM_allocaligna(name, size, align, offset, flags, true);
}

void* MEM_alloc(const char* name, int size, int flags) {
    return MEM_allocaligna(name, size, 0, 0, flags, true);
}

void* MEM_allocz(const char* name, int size, int flags) {
    return MEM_allocaligna(name, size, 0, 0, flags, false);
}
}
