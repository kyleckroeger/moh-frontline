/* CMemoryCard::RewindSaveGameList and Format (a fragment of MemoryCardGC.cpp):
   rewind the save-game index and recount the saves; mount the card, format
   it and unmount it, polling each asynchronous result while servicing disc
   errors. CMemoryCard is named by the mangled symbols; the members are
   inferred from offsets (slot at +4, save-game index at +32, mounted flags
   at +290, card work area at +320). The return types are inferred. */
extern "C" {
long CARDMountAsync(long, void*, void*, void*);
long CARDFormatAsync(long, void*);
long CARDGetResultCode(long);
long CARDUnmount(long);
}

void DebugMsg(const char*, ...);
void MohDVDErrorTask(int, int);

class CMemoryCard {
public:
    void RewindSaveGameList();
    bool Format();
    int GetNumSaveGames();

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[24];
    short m_saveGameIndex;
    unsigned char unknown00022[256];
    bool m_mounted[2];
    unsigned char unknown00124[28];
    unsigned char m_workArea[81920];
};

void CMemoryCard::RewindSaveGameList() {
    m_saveGameIndex = 0;
    GetNumSaveGames();
}

bool CMemoryCard::Format() {
    long result;
    long slot;

    CARDMountAsync(m_slot, m_workArea, 0, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    result = CARDFormatAsync(m_slot, 0);
    if (result < 0) {
        DebugMsg("Format failed with error %d\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return false;
    }
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    if (result < 0) {
        DebugMsg("Format failed with error %d\n", result);
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
        return false;
    }
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    return true;
}
