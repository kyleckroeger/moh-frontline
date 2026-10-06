// Block pools: fixed-size elements carved from pools that are chained to a
// BPoolMan, with a circular free list. BPoolMan and BPool are named by the
// mangled AddNewPool symbol; members are inferred from offsets.
struct BNode {
    BNode* next;
};

struct BPool {
    BPool* next;
    unsigned int count;
};

struct BPoolMan {
    BPool* next;
    BPool* tail;
    BNode* freeHead;
    BNode* freeTail;
    unsigned int size;
};

extern "C" {
extern int mb_default;
void* MEM_alloc(const char*, int, int);
void CON_BPoolMan(BPoolMan*, unsigned int, unsigned int);
}

static void AddNewPool(BPoolMan* man, BPool* pool, unsigned int count);

extern "C" BPoolMan* NEW_BPoolMan(unsigned int count, unsigned int size) {
    BPoolMan* man = (BPoolMan*)MEM_alloc("BPoolMan", count * size + 96, mb_default);

    CON_BPoolMan(man, count, size);
    return man;
}

extern "C" void CON_BPoolMan(BPoolMan* man, unsigned int count, unsigned int size) {
    size = size > 4 ? size : 4;
    size = size + 3 & ~3;
    count = count > 1 ? count : 1;
    man->next = (BPool*)man;
    man->tail = (BPool*)man;
    man->freeHead = (BNode*)&man->freeHead;
    man->freeTail = (BNode*)&man->freeHead;
    man->size = size;
    AddNewPool(man, (BPool*)((char*)man + 32), count);
}

extern "C" void BFree(BPoolMan* man, void* element) {
    man->freeTail->next = (BNode*)element;
    man->freeTail = (BNode*)element;
    ((BNode*)element)->next = (BNode*)&man->freeHead;
}

static void AddNewPool(BPoolMan* man, BPool* pool, unsigned int count) {
    unsigned int i;
    BNode* node;

    pool->count = count;
    man->tail->next = pool;
    man->tail = pool;
    pool->next = (BPool*)man;
    for (i = 0; i < count; i++) {
        node = (BNode*)((char*)pool + i * man->size + 32);
        man->freeTail->next = node;
        man->freeTail = node;
        node->next = (BNode*)&man->freeHead;
    }
}
