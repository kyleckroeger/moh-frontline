/* CMemoryCard::Is8KCard, IsAnyDeviceInPort and IsEncodingCorrect (a fragment
   of MemoryCardGC.cpp): probe a slot for the sector size and for any device,
   unmounting the card when it is gone, and try a mount into the work area
   (+320) to see whether the card's encoding is accepted. CMemoryCard is
   named by the mangled symbols; the members are inferred from offsets (slot
   at +4, per-slot mounted flags at +290, work area at +320). The card functions take the SDK's s32 (long). */
extern "C" {
long CARDCheckAsync(long, void*);
long CARDGetResultCode(long);
long CARDFreeBlocks(long, long*, long*);
long CARDUnmount(long);
long CARDProbeEx(long, long*, long*);
long CARDProbe(long);
long CARDMountAsync(long, void*, void*, void*);
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
    bool IsEncodingCorrect(int);

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[282];
    bool m_mounted[2];
    unsigned char unknown00124[28];
    unsigned char m_workArea[81920];
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

bool CMemoryCard::IsEncodingCorrect(int slot) {
    if (m_mounted[slot]) {
        CARDUnmount(slot);
        m_mounted[slot] = false;
    }
    long result = CARDMountAsync(slot, m_workArea, 0, 0);
    if (result < 0) {
        DebugMsg("Mount failed. Error code %d returned\n", result);
        return true;
    }
    while ((result = CARDGetResultCode(slot)) == -1)
        ;
    CARDUnmount(slot);
    m_mounted[slot] = false;
    return result != -13;
}
