// A fragment of ShellMenu.cpp (0x800df0ec): CShellMenu::GetGalleryMoviesUnlocked
// copies the unlock flags of the twelve gallery movies (+5116) and of five
// further gallery entries (+5128) out as integers and returns the movie
// count. CShellMenu and the function are named by the mangled symbols; the
// members, what the second list holds and the result type are inferred. The
// rest of the file is not part of this unit.
extern "C" void* memcpy(void*, const void*, unsigned long);

class CShellMenu {
public:
    int GetGalleryMoviesUnlocked(int*, int*);

    unsigned char unknown0000[5116];
    unsigned char m_moviesUnlocked[12];
    unsigned char m_extrasUnlocked[5];
};

int CShellMenu::GetGalleryMoviesUnlocked(int* movies, int* extras) {
    int movieFlags[12];
    int extraFlags[5];

    movieFlags[0] = m_moviesUnlocked[0];
    movieFlags[1] = m_moviesUnlocked[1];
    movieFlags[2] = m_moviesUnlocked[2];
    movieFlags[3] = m_moviesUnlocked[3];
    movieFlags[4] = m_moviesUnlocked[4];
    movieFlags[5] = m_moviesUnlocked[5];
    movieFlags[6] = m_moviesUnlocked[6];
    movieFlags[7] = m_moviesUnlocked[7];
    movieFlags[8] = m_moviesUnlocked[8];
    movieFlags[9] = m_moviesUnlocked[9];
    movieFlags[10] = m_moviesUnlocked[10];
    movieFlags[11] = m_moviesUnlocked[11];
    memcpy(movies, movieFlags, sizeof(movieFlags));
    extraFlags[0] = m_extrasUnlocked[0];
    extraFlags[1] = m_extrasUnlocked[1];
    extraFlags[2] = m_extrasUnlocked[2];
    extraFlags[3] = m_extrasUnlocked[3];
    extraFlags[4] = m_extrasUnlocked[4];
    memcpy(extras, extraFlags, sizeof(extraFlags));
    return 12;
}
