// A fragment of ShellMenu.cpp (0x800df814): CShellMenu's music and
// sound-effect volume setters, stored in the audio options record at +40
// through its inline setter, which checks its this pointer (the target's null
// check of the record's address). CShellMenu and the functions are named by
// the mangled symbols; the member and the record view are inferred (see
// ShellMenu_audio.cpp). SetupAudioSettings after this and the rest of the
// file are not part of this unit.
struct AudioOptionsView {
    signed char m_settings[3];

    void Set(int index, signed char value) {
        if (this)
            m_settings[index] = value;
    }
};

class CShellMenu {
public:
    void SetMusicValue(int);
    void SetSoundFXValue(int);

    unsigned char unknown0000[40];
    AudioOptionsView m_audio;
};

void CShellMenu::SetMusicValue(int value) {
    m_audio.Set(1, value);
}

void CShellMenu::SetSoundFXValue(int value) {
    m_audio.Set(0, value);
}
