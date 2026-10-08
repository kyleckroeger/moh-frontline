// A fragment of ShellMenu.cpp (0x800e2338): CShellMenu::LoadData restores the
// shell's saved data (everything before +6368) from the loader's buffer when
// the loader holds valid data (its flag set and the 0xDEADBEEF marker), and
// clears it again if the data's CRC check fails; the validity is kept at
// +6380. SaveData stores the CRC of the saved data at +6364 (the inferred
// DataCRC helper, the same CRC-32 code CheckDataCRC inlines) and copies the
// saved data to the loader's buffer, marking it valid. CShellMenu, LoadElf
// and the functions are named by the mangled symbols; the members, the loader
// fields and the CheckDataCRC parameter meanings are inferred. The rest of
// the file is not part of this unit.
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

// Inferred view of the heap CRC model CheckDataCRC builds (only inlined code
// survives; no symbols): the fields and steps follow the table-less CRC
// model (width, mask, polynomial, initial value, reflection, final xor,
// register).
class CRCModelView {
public:
    CRCModelView() {
        m_width = 32;
        m_poly = 0x04C11DB7;
        m_init = 0xFFFFFFFF;
        m_xorOut = 0xFFFFFFFF;
        m_reflect = 1;
        m_mask = 0xFFFFFFFF;
        m_reg = m_init;
    }
    static unsigned long Reflect(unsigned long v, unsigned long bits) {
        unsigned long t = v;
        for (int i = 0; i < bits; i++) {
            if (t & 1)
                v |= 1 << ((bits - 1) - i);
            else
                v &= ~(1 << ((bits - 1) - i));
            t >>= 1;
        }
        return v;
    }
    void Next(int ch) {
        unsigned long uch = (unsigned long)ch;
        unsigned long topbit = 1 << (m_width - 1);
        if (m_reflect)
            uch = Reflect(uch, 8);
        m_reg ^= uch << (m_width - 8);
        for (int i = 0; i < 8; i++) {
            if (m_reg & topbit)
                m_reg = (m_reg << 1) ^ m_poly;
            else
                m_reg <<= 1;
            m_reg &= m_mask;
        }
    }
    void Block(unsigned char* p, unsigned long len) {
        while (len--)
            Next(*p++);
    }
    unsigned long Crc() {
        if (m_reflect)
            return m_xorOut ^ Reflect(m_reg, m_width);
        return m_xorOut ^ m_reg;
    }

    unsigned long m_width;
    unsigned long m_mask;
    unsigned long m_poly;
    unsigned long m_init;
    unsigned long m_reflect;
    unsigned long m_xorOut;
    unsigned long m_reg;
};

class CShellMenu {
public:
    void LoadData();
    void SaveData();
    bool CheckDataCRC(unsigned char*, int, unsigned long) const;
    inline unsigned long DataCRC(unsigned char*, unsigned long) const;

    unsigned char m_savedData[6364];
    unsigned long m_crc;
    unsigned char m_afterSaved[12]; /* first member after the saved data */
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

unsigned long CShellMenu::DataCRC(unsigned char* data, unsigned long size) const {
    CRCModelView* crc = new CRCModelView;
    if (!data) {
        data = (unsigned char*)this;
        size = (unsigned char*)&m_crc - (unsigned char*)this;
    }
    for (; size; size--) {
        crc->Next(*data);
        data++;
    }
    unsigned long value = crc->Crc();
    delete crc;
    return value;
}

void CShellMenu::SaveData() {
    m_crc = DataCRC(0, 0);
    memcpy(LoadElf::Get(), this, (unsigned char*)&m_afterSaved - (unsigned char*)this);
    LoadElf* elf = LoadElf::Get();
    elf->m_hasData = true;
    elf->m_marker = 0xDEADBEEF;
}
