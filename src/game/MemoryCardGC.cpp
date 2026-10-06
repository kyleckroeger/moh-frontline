// CMemoryCard, the last functions of the file: the empty Initialize and
// destructor, and the constructor that initialises the card library once,
// clears the mounted flags and clears a 32-byte block. CMemoryCard is named
// by the mangled symbols and SysCardInit by its symbol; the members are
// inferred from offsets. OpenSlot and QuerySlot before them are not part of
// the unit (the target keeps the slot in two registers; draft in
// scratch/lib/MemoryCardGC_wip.cpp).
extern "C" {
void* memset(void*, int, unsigned long);
void CARDInit();
}

static bool SysCardInit;

class CMemoryCard {
public:
    CMemoryCard();
    ~CMemoryCard();
    void Initialize();

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[282];
    bool m_mounted[2];
    unsigned char unknown00124[81948];
    unsigned char m_cleared[32];
};

void CMemoryCard::Initialize() {
}

CMemoryCard::~CMemoryCard() {
}

CMemoryCard::CMemoryCard() {
    if (!SysCardInit) {
        CARDInit();
        SysCardInit = true;
    }
    m_mounted[0] = false;
    m_mounted[1] = false;
    memset(m_cleared, 0, 32);
}
