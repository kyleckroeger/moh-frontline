// A fragment of ShellMenu.cpp (0x800e4bb8): CShellMenu's memory-card queries
// forwarded to the card at +6412: the corrupt file's name (copied from a
// buffer in the card object), the free file count, whether a device is in the
// port, the card's block count, the encoding check and the 8K sector check.
// CShellMenu, CMemoryCard and the functions are named by the mangled symbols;
// the members, the name buffer's offset and the result types are inferred.
// The rest of the file is not part of this unit.
extern "C" char* strcpy(char*, const char*);

class CMemoryCard {
public:
    int GetFilesFree(int);
    int IsAnyDeviceInPort(int);
    int GetMaxBlocksOnCard(int);
    int IsEncodingCorrect(int);
    int Is8KCard(int);

    unsigned char unknown00000[82240];
    char m_corruptFileName[32];
};

class CShellMenu {
public:
    void MemoryCardGetCorruptFileName(char*, int);
    int MemoryCardNumFilesFree(int);
    int MemoryCardIsAnyObjectInSlot(int);
    int MemoryCardGetMaxBlocks(int);
    int MemoryCardIsEncodingCorrect(int);
    int MemoryCardIs8KSectorSize(int);

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
};

void CShellMenu::MemoryCardGetCorruptFileName(char* name, int) {
    strcpy(name, m_card->m_corruptFileName);
}

int CShellMenu::MemoryCardNumFilesFree(int port) {
    return m_card->GetFilesFree(port);
}

int CShellMenu::MemoryCardIsAnyObjectInSlot(int port) {
    return m_card->IsAnyDeviceInPort(port);
}

int CShellMenu::MemoryCardGetMaxBlocks(int port) {
    return m_card->GetMaxBlocksOnCard(port);
}

int CShellMenu::MemoryCardIsEncodingCorrect(int port) {
    return m_card->IsEncodingCorrect(port);
}

int CShellMenu::MemoryCardIs8KSectorSize(int port) {
    return m_card->Is8KCard(port);
}
