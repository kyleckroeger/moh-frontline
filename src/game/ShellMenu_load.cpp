// A fragment of ShellMenu.cpp (0x800e2338): CShellMenu::LoadData restores the
// shell's saved data (everything before +6368) from the loader's buffer when
// the loader holds valid data (its flag set and the 0xDEADBEEF marker), and
// clears it again if the data's CRC check fails; the validity is kept at
// +6380. CShellMenu, LoadElf and the functions are named by the mangled
// symbols; the members, the loader fields and the CheckDataCRC parameter
// meanings are inferred. The rest of the file is not part of this unit.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
}

class LoadElf {
public:
    static LoadElf* Get();

    unsigned char unknown0000[12300];
    bool m_hasData;
    unsigned char unknown300d[3];
    unsigned int m_marker;
};

class CShellMenu {
public:
    void LoadData();
    bool CheckDataCRC(unsigned char*, int, unsigned long) const;

    unsigned char m_savedData[6368];
    unsigned char m_afterSaved[12]; /* first member after the saved data */;
    bool m_dataLoaded;
};

void CShellMenu::LoadData() {
    LoadElf* elf = LoadElf::Get();
    bool loaded = false;
    int size;

    if (elf->m_hasData && elf->m_marker == 0xDEADBEEF)
        loaded = true;
    m_dataLoaded = loaded;
    if (m_dataLoaded) {
        size = (unsigned char*)&m_afterSaved - (unsigned char*)this;
        memcpy(this, LoadElf::Get(), size);
        if (!CheckDataCRC(0, 0, 0))
            memset(this, 0, size);
    }
}
