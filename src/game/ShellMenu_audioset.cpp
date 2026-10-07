// A fragment of ShellMenu.cpp (0x800df814): CShellMenu's music and
// sound-effect volume setters, stored in the audio options record at +40
// through its inline setter, which checks its this pointer (the target's null
// check of the record's address), and SetupAudioSettings: stores all three
// settings, sets the sound volumes from the first two (steps 1 to 7 as 0 to
// 1), the stream volume from the music setting (0 to 127, under the
// sound-system lock) and the sound mode from the third. Its constants are
// entries of the file's .sdata2 pool. CShellMenu, CSoundSysLock,
// CSoundStream and the functions are named by the mangled symbols; the
// member, the record view and the sound-mode values are inferred (see
// ShellMenu_audio.cpp). The rest of the file is not part of this unit.
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

class CSoundStream {
public:
    void SetVolume(int);
};

extern CSoundStream* pSoundStream;

enum ESoundMode {};

void SoundSetVolumes(float, float);
void SoundSetMode(ESoundMode);
extern "C" void OSSetSoundMode(unsigned long);

struct AudioOptionsView {
    signed char m_settings[3];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
    void Set(int index, unsigned char value) {
        if (this)
            m_settings[index] = value;
    }
};

class CShellMenu {
public:
    void SetMusicValue(int);
    void SetSoundFXValue(int);
    void SetupAudioSettings(int*);

    unsigned char unknown0000[40];
    AudioOptionsView m_audio;
};

void CShellMenu::SetMusicValue(int value) {
    m_audio.Set(1, value);
}

void CShellMenu::SetSoundFXValue(int value) {
    m_audio.Set(0, value);
}

void CShellMenu::SetupAudioSettings(int* settings) {
    m_audio.Set(0, settings[0]);
    m_audio.Set(1, settings[1]);
    m_audio.Set(2, settings[2]);
    float effects = (m_audio.Get(0) - 1) / 6.0f;
    float music = (m_audio.Get(1) - 1) / 6.0f;
    SoundSetVolumes(effects, music);
    {
        CSoundSysLock lock;
        if (pSoundStream)
            pSoundStream->SetVolume(127.0f * (m_audio.Get(1) - 1) / 6.0f);
    }
    switch (m_audio.Get(2)) {
    case 0:
        SoundSetMode((ESoundMode)1);
        OSSetSoundMode(1);
        break;
    case 1:
        SoundSetMode((ESoundMode)2);
        OSSetSoundMode(1);
        break;
    case 2:
        SoundSetMode((ESoundMode)0);
        OSSetSoundMode(0);
        break;
    }
}
