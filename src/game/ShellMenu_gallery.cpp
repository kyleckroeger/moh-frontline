// A fragment of ShellMenu.cpp (0x800def28): CShellMenu::SetGlobalCheatStructure
// clears the first two cheat switches and sets each of the others from
// whether its secret's status is 2 (a switch over the secret index, with its
// jump table in .data; the float beside one switch is reset either way from
// the file's .sdata2 pool), then GetGalleryMoviesUnlocked
// copies the unlock flags of the twelve gallery movies (+5116) and of five
// further gallery entries (+5128) out as integers and returns the movie
// count. CShellMenu and the function are named by the mangled symbols; the
// members, what the second list holds, the cheat view and the result types
// are inferred. The
// rest of the file is not part of this unit.
extern "C" void* memcpy(void*, const void*, unsigned long);

// Inferred: the cheat switches (20 bytes, the size of g_cheats).
struct CheatsView {
    bool data00;
    bool data01;
    bool data02;
    bool data03;
    float data04;
    bool data08;
    bool data09;
    bool data0a;
    bool data0b;
    bool data0c;
    bool data0d;
    bool data0e;
    bool data0f;
    unsigned char unknown10[4];
};

extern CheatsView g_cheats;

class CShellMenu {
public:
    int GetSecretsStatus(int*);
    void SetGlobalCheatStructure();
    int GetGalleryMoviesUnlocked(int*, int*);

    unsigned char unknown0000[5116];
    unsigned char m_moviesUnlocked[12];
    unsigned char m_extrasUnlocked[5];
};

void CShellMenu::SetGlobalCheatStructure() {
    int status[10];

    GetSecretsStatus(status);
    g_cheats.data00 = false;
    g_cheats.data01 = false;
    for (int i = 1; i <= 10; i++) {
        int state = status[i - 1];
        switch (i) {
        case 10:
            if (state == 2)
                g_cheats.data02 = true;
            else
                g_cheats.data02 = false;
            break;
        case 9:
            if (state == 2) {
                g_cheats.data03 = true;
                g_cheats.data04 = 0.0f;
            } else {
                g_cheats.data03 = false;
                g_cheats.data04 = 0.0f;
            }
            break;
        case 3:
            if (state == 2)
                g_cheats.data08 = true;
            else
                g_cheats.data08 = false;
            break;
        case 8:
            if (state == 2)
                g_cheats.data09 = true;
            else
                g_cheats.data09 = false;
            break;
        case 7:
            if (state == 2)
                g_cheats.data0a = true;
            else
                g_cheats.data0a = false;
            break;
        case 1:
            if (state == 2)
                g_cheats.data0b = true;
            else
                g_cheats.data0b = false;
            break;
        case 2:
            if (state == 2)
                g_cheats.data0c = true;
            else
                g_cheats.data0c = false;
            break;
        case 6:
            if (state == 2)
                g_cheats.data0d = true;
            else
                g_cheats.data0d = false;
            break;
        case 4:
            if (state == 2)
                g_cheats.data0e = true;
            else
                g_cheats.data0e = false;
            break;
        case 5:
            if (state == 2)
                g_cheats.data0f = true;
            else
                g_cheats.data0f = false;
            break;
        }
    }
}

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
