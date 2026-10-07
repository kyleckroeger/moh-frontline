// A fragment of ShellMenu.cpp (0x800e1f48): CShellMenu::FadeTheVolume fades
// the sound stream to a volume over 1500 (under the sound-system lock), then
// keeps updating the sound system until the stream reaches that volume, and
// returns the volume it had (0 without a stream). CShellMenu, CSoundStream,
// CSoundSysLock and the functions are named by the mangled symbols; the
// parameter meanings are inferred. The rest of the file is not part of this
// unit.
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

class CShellMenu {
public:
    int FadeTheVolume(int);
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
