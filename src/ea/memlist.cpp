// Free list of a REAL memory class: a circular list through the free blocks'
// list links, kept in address order, with the class's 'BS' sentinel (size
// 0x7fffffff) as head and tail. MEMBLOCK and MEMCLASS are named by the mangled
// symbols; their members are inferred from offsets and are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
    MEMBLOCK* freenext;
    MEMBLOCK* freeprev;
};

struct MEMCLASS {
    char name[8];
    MEMBLOCK* low;
    MEMBLOCK* high;
    MEMBLOCK free;
};

static const int MB_FREE = 0x4000;

MEMBLOCK* FREE_find(MEMCLASS* cls, int size, int fromtop) {
    MEMBLOCK* block = &cls->free;

    if (!fromtop) {
        do
            block = block->freenext;
        while (size > block->size);
    } else {
        do
            block = block->freeprev;
        while (size > block->size);
    }
    if (block->magic == 0x4253)
        return 0;
    return block;
}

MEMBLOCK* FREE_findlargest(MEMCLASS* cls, int size, int fromtop) {
    MEMBLOCK* block = &cls->free;
    MEMBLOCK* largest = 0;
    int best = 0 > size - 1 ? 0 : size - 1;
    int blocksize;
    if (!fromtop) {
        for (;;) {
            block = block->freenext;
            blocksize = block->size - 16;
            if (blocksize > best) {
                if (block->magic == 0x4253)
                    return largest;
                largest = block;
                best = blocksize;
            }
        }
    } else {
        for (;;) {
            block = block->freeprev;
            blocksize = block->size - 16;
            if (blocksize > best) {
                if (block->magic == 0x4253)
                    return largest;
                largest = block;
                best = blocksize;
            }
        }
    }
}

int FREE_gettotalfree(MEMCLASS* cls, int fromtop) {
    MEMBLOCK* block = &cls->free;
    int total = 0;

    if (!fromtop) {
        for (;;) {
            block = block->freenext;
            if (block->magic == 0x4253)
                return total;
            total += block->size - 16;
        }
    } else {
        for (;;) {
            block = block->freeprev;
            if (block->magic == 0x4253)
                return total;
            total += block->size - 16;
        }
    }
}

void FREE_add(MEMCLASS* cls, MEMBLOCK* block) {
    MEMBLOCK* next = &cls->free;
    MEMBLOCK* prev = next;
    int size = (char*)block->next - (char*)block;

    if ((unsigned int)block > (unsigned int)((char*)next->freenext + ((char*)next->freeprev - (char*)next->freenext) / 2)) {
        do
            prev = prev->freeprev;
        while (block < prev);
        next = prev->freenext;
    } else {
        do
            next = next->freenext;
        while (block > next);
        prev = next->freeprev;
    }
    block->freenext = next;
    block->freeprev = prev;
    block->size = size;
    prev->freenext = block;
    next->freeprev = block;
    block->flags |= MB_FREE;
    block->magic = 0x4246;
}

void FREE_remove(MEMBLOCK* block) {
    MEMBLOCK* prev = block->freeprev;
    MEMBLOCK* next = block->freenext;

    prev->freenext = next;
    next->freeprev = prev;
    block->flags &= ~0x4000;
    block->magic = 0x424f;
}
