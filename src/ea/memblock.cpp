// MEM_initblock: write a block header ('BM', flags, size and the neighbour
// links) and return the block's total length. The name and tail-size
// arguments are not stored. MEMBLOCK is named by the mangled symbol; its
// members are inferred from offsets and are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
    MEMBLOCK* freenext;
    MEMBLOCK* freeprev;
};

int MEM_initblock(MEMBLOCK* block, const char* name, int size, int tailsize, int flags, MEMBLOCK* prev,
                  MEMBLOCK* next) {
    block->magic = 0x424d;
    block->flags = flags;
    block->size = size;
    block->next = next;
    block->prev = prev;
    return (char*)block + size + 16 - (char*)block;
}
