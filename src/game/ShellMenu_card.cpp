// A fragment of ShellMenu.cpp (0x800e5380): CShellMenu's memory-card
// wrappers. Each selects the card port first (already current, or opened)
// and then rewinds the save-game list, checks for preferences, formats or
// checks the format; MemoryCardQueryPort only opens the slot. The file name is
// this project's; the original record is ShellMenu.cpp and
// GetWeaponMessageForTotalStats after these is not reconstructed. The classes
// and functions are named by the mangled symbols; CShellMenu is an inferred
// view (the card at +6412), the port selection is an inferred inline helper,
// and the result types are inferred.
class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    void RewindSaveGameList();
    bool ArePrefsPresent();
    bool Format();
    bool IsFormated();
};

class CShellMenu {
public:
    void MemoryCardRewindSaveGameList(int);
    bool MemoryCardArePrefsPresent(int);
    bool MemoryCardFormat(int);
    bool MemoryCardIsFormated(int);
    bool MemoryCardQueryPort(int);

    bool SelectCardPort(int port) {
        if (port != m_card->GetPort() && !m_card->OpenSlot(port))
            return false;
        return true;
    }

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
};

void CShellMenu::MemoryCardRewindSaveGameList(int port) {
    if (SelectCardPort(port))
        m_card->RewindSaveGameList();
}

bool CShellMenu::MemoryCardArePrefsPresent(int port) {
    if (!SelectCardPort(port))
        return false;
    return m_card->ArePrefsPresent();
}

bool CShellMenu::MemoryCardFormat(int port) {
    if (!SelectCardPort(port))
        return false;
    return m_card->Format();
}

bool CShellMenu::MemoryCardIsFormated(int port) {
    if (!SelectCardPort(port))
        return false;
    return m_card->IsFormated();
}

bool CShellMenu::MemoryCardQueryPort(int port) {
    return m_card->OpenSlot(port);
}
