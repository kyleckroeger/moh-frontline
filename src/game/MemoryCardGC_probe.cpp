/* CMemoryCard::Is8KCard and IsAnyDeviceInPort (a fragment of
   MemoryCardGC.cpp): probe a slot for the sector size and for any device,
   unmounting the card when it is gone. CMemoryCard is named by the mangled symbols; the
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

bool CMemoryCard::Is8KCard(int slot) {
    long memSize;
    long sectorSize;
    long result;
    while ((result = CARDProbeEx(slot, &memSize, &sectorSize)) == -1)
        ;
    if (result < 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return true;
    }
    if (sectorSize == 8192)
        return true;
    return false;
}

bool CMemoryCard::IsAnyDeviceInPort(int slot) {
    long result;
    while ((result = CARDProbe(slot)) == -1)
        ;
    if (result == 0) {
        if (m_mounted[slot])
            CARDUnmount(slot);
        m_mounted[slot] = false;
        return false;
    }
    return true;
}
