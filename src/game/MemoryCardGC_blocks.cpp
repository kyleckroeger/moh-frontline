/* CMemoryCard::IsFormated and GetMaxBlocksOnCard (a fragment of
   MemoryCardGC.cpp). IsFormated unmounts the slot if it is mounted, mounts it
   again (returning the mount result when it is not 1) and queries the sector
   size into the member at +28 (0 on failure, else 1); the result type is
   inferred as int (it returns the mount result unchanged); the messages are
   entries of the file's .rodata pool. GetMaxBlocksOnCard returns the
   card's size in Mbit converted to blocks of the sector size, less the five
   system blocks. CMemoryCard is named by the mangled symbols; the
   members are inferred from offsets (slot at +4, per-slot mounted flags at
   +290). The card functions take the SDK's s32 (long). */
extern "C" {
long CARDCheckAsync(long, void*);
long CARDGetResultCode(long);
long CARDFreeBlocks(long, long*, long*);
long CARDUnmount(long);
long CARDProbeEx(long, long*, long*);
long CARDProbe(long);
long CARDGetSectorSize(long, unsigned long*);
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
    int IsFormated();

    unsigned char unknown00000[4];
    int m_slot;
    unsigned char unknown00008[20];
    unsigned long m_sectorSize;
    unsigned char unknown00020[258];
    bool m_mounted[2];
};

int CMemoryCard::IsFormated() {
    if (m_mounted[m_slot]) {
        CARDUnmount(m_slot);
        m_mounted[m_slot] = false;
    }
    DebugMsg("Attempting to mount a card into slot %d\n", m_slot);
    int result = MountPortIfRequired(m_slot);
    if (result != 1)
        return result;
    result = CARDGetSectorSize(m_slot, &m_sectorSize);
    if (result < 0) {
        DebugMsg("CARDGetSectorSize() failed on error code %d\n", result);
        return 0;
    }
    return 1;
}

int CMemoryCard::GetMaxBlocksOnCard(int slot) {
    long memSize = 0;
    long sectorSize = 8192;
    while (CARDProbeEx(slot, &memSize, &sectorSize) == -1)
        ;
    return (memSize << 17) / sectorSize - 5;
}
