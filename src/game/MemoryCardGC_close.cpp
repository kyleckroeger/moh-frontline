/* CMemoryCard::Close (a fragment of MemoryCardGC.cpp): close the open save
   file of a mounted card and unmount it, reporting the result. CMemoryCard
   is named by the mangled symbols and CARDFileInfo by the SDK; the members
   are inferred from offsets (slot at +4, file info at +8, mounted flags at
   +290). The return type is inferred. */
struct CARDFileInfo {
    unsigned char unknown00[20];
};

extern "C" {
long CARDClose(CARDFileInfo*);
long CARDUnmount(long);
}

void DebugMsg(const char*, ...);

class CMemoryCard {
public:
    bool Close();

    unsigned char unknown00000[4];
    int m_slot;
    CARDFileInfo m_fileInfo;
    unsigned char unknown0001C[262];
    bool m_mounted[2];
};

bool CMemoryCard::Close() {
    long result;

    if (!m_mounted[m_slot])
        return true;
    result = CARDClose(&m_fileInfo);
    CARDUnmount(m_slot);
    m_mounted[m_slot] = false;
    if (result != 0) {
        DebugMsg("File failed to close with error %d\n", result);
        return false;
    }
    DebugMsg("File closed successfuly\n");
    return true;
}
