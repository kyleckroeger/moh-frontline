// A fragment of ShellMenu.cpp (0x800e1f48): CShellMenu::FadeTheVolume fades
// the sound stream to a volume over 1500 (under the sound-system lock), then
// keeps updating the sound system until the stream reaches that volume, and
// returns the volume it had (0 without a stream). CShellMenu::CheckDataCRC
// computes the CRC-32 (polynomial 0x04C11DB7, reflected, initial value and
// final xor all ones) of a buffer, by default the shell's saved data before
// its stored CRC at +6364, and compares it with the expected value (by
// default that stored CRC). CShellMenu, CSoundStream, CSoundSysLock and the
// functions are named by the mangled symbols; the parameter meanings are
// inferred. DataCRC is an inferred inline helper (SaveData inlines the same
// code with the default buffer); the original does not inline CheckDataCRC
// into LoadData, which is kept in another fragment. The rest of the file is
// not part of this unit.
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

class CSoundStream {
public:
    int GetVolume() const;
    void FadeVolume(int, int);
};

extern CSoundStream* pSoundStream;

void UpdateREAL();

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
    int FadeTheVolume(int);
    bool CheckDataCRC(unsigned char*, int, unsigned long) const;
    inline unsigned long DataCRC(unsigned char*, unsigned long) const;

    unsigned char m_savedData[6364];
    unsigned long m_crc;
};

int CShellMenu::FadeTheVolume(int volume) {
    int previous = 0;

    if (pSoundStream) {
        CSoundSysLock lock;
        previous = pSoundStream->GetVolume();
        pSoundStream->FadeVolume(1500, volume);
    }
    if (pSoundStream) {
        int current;
        do {
            UpdateREAL();
            CSoundSysLock lock;
            current = pSoundStream->GetVolume();
        } while (current != volume);
    }
    return previous;
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

bool CShellMenu::CheckDataCRC(unsigned char* data, int size, unsigned long expected) const {
    if (!data) {
        data = (unsigned char*)this;
        size = (unsigned char*)&m_crc - data;
        expected = m_crc;
    }
    unsigned long crc = DataCRC(data, size);
    return crc == expected;
}
