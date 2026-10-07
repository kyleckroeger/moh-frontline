// A fragment of ShellMenu.cpp (0x800e7160): turning the shell off (fading the
// sound stream out, which inlines FadeTheVolume, defined earlier in the file;
// then setting the flag at +6569, purging and destroying the stream and
// clearing the stream request), whether the shell is on, and drawing the
// shell's interface studio while it is on. CShellMenu, IStudio, CSoundStream,
// CSoundSysLock and the functions are named by the mangled symbols; the
// members are inferred. FadeTheVolume itself is in ShellMenu_fade.cpp; the
// rest of the file is not part of this unit.
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

class CSoundStream {
public:
    ~CSoundStream();
    int GetVolume() const;
    void FadeVolume(int, int);
    void PurgeQueue();
};

extern CSoundStream* pSoundStream;
extern int g_soundStreamRequest;

void UpdateREAL();

class IStudio {
public:
    void iSirenDrawProcessFnc(unsigned long);

    unsigned char data[24];
};

class CShellMenu {
public:
    int FadeTheVolume(int);
    void TurnShellOff();
    bool bIsShellOn();
    void ShellDraw(unsigned long);

    unsigned char unknown0000[6384];
    IStudio m_studio;
    bool m_shellOn;
    unsigned char unknown1909[160];
    bool m_flag6569;
};

inline int CShellMenu::FadeTheVolume(int volume) {
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

void CShellMenu::TurnShellOff() {
    FadeTheVolume(0);
    m_flag6569 = true;
    if (pSoundStream) {
        pSoundStream->PurgeQueue();
        g_soundStreamRequest = -1;
        delete pSoundStream;
        pSoundStream = 0;
    }
    m_shellOn = false;
}

bool CShellMenu::bIsShellOn() {
    return m_shellOn;
}

void CShellMenu::ShellDraw(unsigned long time) {
    if (m_shellOn)
        m_studio.iSirenDrawProcessFnc(time);
}
