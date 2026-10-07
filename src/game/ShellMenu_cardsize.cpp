// A fragment of ShellMenu.cpp (0x800e421c): CShellMenu's memory-card space
// queries: the free space on the card in a port (0 unless the port is
// current or opens) and the save sizes of the preferences and the game (both
// the card's overhead size with the flag clear). CShellMenu, CMemoryCard and
// the functions are named by the mangled symbols; the card member (+6412),
// the port selection inline (as in ShellMenu_card.cpp) and the result types
// are inferred. The rest of the file is not part of this unit.
class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    int GetAvailableMemory();
    int GetSaveGameOverheadSize(bool);
};

class CShellMenu {
public:
    int MemoryCardFreeSpace(int);
    int MemoryCardSavePrefsSize();
    int MemoryCardSaveGameSize();

    bool SelectCardPort(int port) {
        if (port != m_card->GetPort() && !m_card->OpenSlot(port))
            return false;
        return true;
    }

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
};

int CShellMenu::MemoryCardFreeSpace(int port) {
    if (!SelectCardPort(port))
        return 0;
    return m_card->GetAvailableMemory();
}

int CShellMenu::MemoryCardSavePrefsSize() {
    return m_card->GetSaveGameOverheadSize(false);
}

int CShellMenu::MemoryCardSaveGameSize() {
    return m_card->GetSaveGameOverheadSize(false);
}
