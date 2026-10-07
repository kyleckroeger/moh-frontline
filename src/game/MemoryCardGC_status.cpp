/* CMemoryCard::GetCheckCardStatus and GetFilesFree (a fragment of
   MemoryCardGC.cpp): run a card check, polling the result while servicing
   disc errors, and return the free file count of a mounted card, unmounting
   it when the query fails. CMemoryCard is named by the mangled symbols; the
   members are inferred from offsets (slot at +4, per-slot mounted flags at
   +290). The card functions take the SDK's s32 (long). */
extern "C" {
long CARDCheckAsync(long, void*);
long CARDGetResultCode(long);
long CARDFreeBlocks(long, long*, long*);
long CARDUnmount(long);
long CARDProbeEx(long, long*, long*);
long CARDProbe(long);
}

void DebugMsg(const char*, ...);
void MohDVDErrorTask(int, int);

class CMemoryCard {
public:
    long GetCheckCardStatus();
    long GetFilesFree(int);
    int GetMaxBlocksOnCard(int);
    int MountPortIfRequired(int);
    bool Is8KCard(int);
    bool IsAnyDeviceInPort(int);

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[282];
    bool m_mounted[2];
};

long CMemoryCard::GetCheckCardStatus() {
    long result;
    long slot;
    CARDCheckAsync(m_slot, 0);
    slot = m_slot;
    while ((result = CARDGetResultCode(slot)) == -1)
        MohDVDErrorTask(0, 0);
    return result;
}

long CMemoryCard::GetFilesFree(int slot) {
    long bytesNotUsed;
    long filesNotUsed;
    long result;
    if (MountPortIfRequired(slot) != 1)
        return 0;
    result = CARDFreeBlocks(slot, &bytesNotUsed, &filesNotUsed);
    if (result < 0) {
        DebugMsg("Free Blocks failed. (%d)\n", result);
        CARDUnmount(slot);
        m_mounted[slot] = false;
        return 0;
    }
    return filesNotUsed;
}
