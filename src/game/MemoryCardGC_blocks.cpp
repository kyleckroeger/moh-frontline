/* CMemoryCard::GetMaxBlocksOnCard (a fragment of MemoryCardGC.cpp): the
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

int CMemoryCard::GetMaxBlocksOnCard(int slot) {
    long memSize = 0;
    long sectorSize = 8192;
    while (CARDProbeEx(slot, &memSize, &sectorSize) == -1)
        ;
    return (memSize << 17) / sectorSize - 5;
}
