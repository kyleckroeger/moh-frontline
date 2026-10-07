// CMemoryCard, the last functions of the file: OpenSlot and QuerySlot, which
// probe a card slot and unmount it when the card is gone, the empty Initialize
// and destructor, and the constructor that initialises the card library once,
// clears the mounted flags and clears a 32-byte block. CMemoryCard is named
// by the mangled symbols and SysCardInit by its symbol; the members are
// inferred from offsets. The card functions take the SDK's s32 (long), which
// is why the slot is copied for those calls.
extern "C" {
void* memset(void*, int, unsigned long);
void CARDInit();
long CARDProbe(long);
long CARDProbeEx(long, long*, long*);
long CARDUnmount(long);
}

static bool SysCardInit;

class CMemoryCard {
public:
    CMemoryCard();
    ~CMemoryCard();
    int OpenSlot(int);
    int QuerySlot(int);
    void Initialize();

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[282];
    bool m_mounted[2];
    unsigned char unknown00124[81948];
    unsigned char m_cleared[32];
};

int CMemoryCard::OpenSlot(int slot) {
    int result;
    m_slot = slot;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    for (;;) {
        while ((result = CARDProbeEx(slot, 0, 0)) == -1)
            ;
        if (result < 0) {
            if (m_mounted[slot])
                CARDUnmount(slot);
            m_mounted[slot] = false;
        }
        if (result == -2)
            return 0;
        return 1;
    }
}

int CMemoryCard::QuerySlot(int slot) {
    int result;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    for (;;) {
        while ((result = CARDProbeEx(slot, 0, 0)) == -1)
            ;
        if (result < 0) {
            if (m_mounted[slot])
                CARDUnmount(slot);
            m_mounted[slot] = false;
        }
        return 1;
    }
}

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
