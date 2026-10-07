// A fragment of ShellMenu.cpp (0x800e79fc): CShellMenu's destructor deletes
// the memory card object (and clears the pointer); the user message table and
// the interface studio members are then destroyed. CShellMenu, CMemoryCard,
// CUserMessageTable and IStudio are named by the mangled symbols; the member
// offsets are inferred (CShellMenu is a non-virtual view). The rest of the
// file is not part of this unit.
class CMemoryCard {
public:
    ~CMemoryCard();
};

class IStudio {
public:
    ~IStudio();

    unsigned char data[28];
};

class CUserMessageTable {
public:
    ~CUserMessageTable();

    unsigned char data[16];
};

class CShellMenu {
public:
    ~CShellMenu();

    unsigned char unknown0000[6384];
    IStudio m_studio;
    CMemoryCard* m_card;
    unsigned char unknown1910[180];
    CUserMessageTable m_messages;
};

CShellMenu::~CShellMenu() {
    if (m_card) {
        delete m_card;
        m_card = 0;
    }
}
