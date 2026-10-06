// MEM_size: the size recorded in an allocation's block header. MEMBLOCK is
// named by mangled symbols elsewhere in the memory library; its members are
// inferred from offsets and are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
};

extern "C" int MEM_size(void* address) {
    return ((MEMBLOCK*)((char*)address - 16))->size;
}
