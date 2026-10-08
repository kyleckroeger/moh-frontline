// A fragment of ShellMenu.cpp (0x800e6f4c): shutting the shell down (deleting
// the two owned objects at +6616 and +6612, unloading the sound modules,
// shutting the interface studio down, deleting the memory card, unloading the
// message table and the shell's big file), turning the shell on (setting the
// flag, creating the sound stream, queueing the stream file when one is named
// and setting its volume from option 1, then setting the two sound volumes
// from options 0 and 1, each (setting - 1) / 6), turning it off (fading the
// sound stream out, which inlines FadeTheVolume, defined earlier in the file;
// then setting the flag at +6569, purging and destroying the stream and
// clearing the stream request), whether the shell is on, and drawing the
// shell's interface studio while it is on. CShellMenu, IStudio, CSoundStream,
// CSoundSysLock, CMemoryCard, CUserMessageTable and the functions are named
// by the mangled symbols; the members, the owned objects' view (its virtual
// table after 28 bytes) and the options view (as in ShellMenu_options.cpp)
// are inferred. FadeTheVolume itself is in ShellMenu_fade.cpp; the
// rest of the file is not part of this unit.
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

class CSoundStream {
public:
    CSoundStream(int, int);
    int QueueFile(const char*, int, int);
    void SetVolume(int);
    ~CSoundStream();
    int GetVolume() const;
    void FadeVolume(int, int);
    void PurgeQueue();

    unsigned char unknown00[32];
};

extern CSoundStream* pSoundStream;
extern int g_soundStreamRequest;
extern char g_soundStreamFilename[];

void SoundSetVolumes(float, float);
void UnloadShellSoundModules();
void UnloadShellBigFile();

class CMemoryCard {
public:
    ~CMemoryCard();
};

class CUserMessageTable {
public:
    void Unload();
};

// Inferred: an object owned by the shell, its virtual table after 28 bytes.
class ShellOwnedView {
    unsigned char unknown00[28];

public:
    virtual ~ShellOwnedView();
};

struct ControllerOptionsView {
    signed char m_settings[4];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
};

void UpdateREAL();

class IStudio {
public:
    void iSirenDrawProcessFnc(unsigned long);
    void iSirenShutDown();

    unsigned char data[24];
};

class CShellMenu {
public:
    int FadeTheVolume(int);
    void ShutdownShell();
    void TurnShellOn();
    void TurnShellOff();
    bool bIsShellOn();
    void ShellDraw(unsigned long);

    unsigned char unknown0000[40];
    ControllerOptionsView m_options;
    unsigned char unknown002c[6340];
    IStudio m_studio;
    bool m_shellOn;
    unsigned char unknown1909[3];
    CMemoryCard* m_memoryCard;
    unsigned char unknown1910[153];
    bool m_flag6569;
    unsigned char unknown19aa[26];
    CUserMessageTable m_messageTable;
    unsigned char unknown19c5[15];
    ShellOwnedView* m_unknown19d4;
    ShellOwnedView* m_unknown19d8;
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

void CShellMenu::ShutdownShell() {
    delete m_unknown19d8;
    delete m_unknown19d4;
    UnloadShellSoundModules();
    m_studio.iSirenShutDown();
    delete m_memoryCard;
    m_memoryCard = 0;
    m_messageTable.Unload();
    UnloadShellBigFile();
}

void CShellMenu::TurnShellOn() {
    m_shellOn = true;
    pSoundStream = new CSoundStream(1, 50);
    if (g_soundStreamFilename[0]) {
        g_soundStreamRequest = pSoundStream->QueueFile(g_soundStreamFilename, 50, 0);
        CSoundSysLock lock;
        pSoundStream->SetVolume(127.0f * (m_options.Get(1) - 1) / 6.0f);
    }
    float first = (m_options.Get(0) - 1) / 6.0f;
    float second = (m_options.Get(1) - 1) / 6.0f;
    SoundSetVolumes(first, second);
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
