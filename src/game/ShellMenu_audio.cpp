// A fragment of ShellMenu.cpp (0x800df530): CShellMenu's audio options (three
// signed byte settings in a record at +40, sound effects first, then music)
// passed to the interface studio. The record's inline accessors check their
// this pointer (-1 for a missing record), as the target's null checks of the
// record's address show. CShellMenu and the function are named by the mangled
// symbols; the member, the record view and the result type are inferred.
// ResetAudio after this and the rest of the file are not part of this unit
// (the music and sound-effect setters are in ShellMenu_audioset.cpp).
extern "C" void* memcpy(void*, const void*, unsigned long);

struct AudioOptionsView {
    signed char m_settings[3];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
};

class CShellMenu {
public:
    int PassAudioSettingToIStudio(int*);

    unsigned char unknown0000[40];
    AudioOptionsView m_audio;
};

int CShellMenu::PassAudioSettingToIStudio(int* out) {
    int settings[3];

    settings[0] = m_audio.Get(0);
    settings[1] = m_audio.Get(1);
    settings[2] = m_audio.Get(2);
    memcpy(out, settings, sizeof(settings));
    return 3;
}
