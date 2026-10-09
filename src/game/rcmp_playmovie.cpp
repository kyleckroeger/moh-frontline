// A fragment of rcmp_main.cpp (0x8010408c): RCMP_PlayMovie creates the
// option-parsing player (its inline constructor sets three scales to 1.0f and
// clears the rest), loads the movie (an inferred inline helper: a message when
// the file does not exist, otherwise an AV_PLAYER for it, 1 MB of buffer,
// allocated through the RCMP system's allocator hook by the class operator
// new), and if that worked marks the player as playing, plays it, deletes it
// (its inline destructor deletes the AV_PLAYER) and clears the playing flag
// through the stale pointer, as the original does. The file name is this
// project's; the original record is rcmp_main.cpp, between
// rcmp_main_delete.cpp and rcmp_main.cpp. The classes, enums and functions are
// named by the mangled symbols; the allocator hook is as in audioplayer.cpp;
// OPTION_PARSING_PLAYER's members, their names and the constructor's store
// order are inferred from the code. The strings and constants are items of
// the file's .rodata and .sdata2 pools.
extern "C" bool FILE_exists(const char*);
void PRINT_string(const char*, ...);

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int);
    void (*m_free)(void*);
    int m_userValue;
};

extern RCMP_SYSTEM rcmp_sys;

class AV_PLAYER {
public:
    enum LOAD_ENUM {};
    enum SOUND_ENUM {};

    AV_PLAYER(const char*, int, const char*, int, LOAD_ENUM, SOUND_ENUM);
    ~AV_PLAYER();
    static void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_userValue); }
    static void operator delete(void*);

    unsigned char unknown00[96];
};
}

class OPTION_PARSING_PLAYER {
public:
    OPTION_PARSING_PLAYER() {
        data2c = 0;
        data30 = 0;
        data31 = 0;
        data32 = 0;
        data2e = 0;
        data2f = 0;
        data2d = 0;
        data33 = 0;
        data34 = 0;
        data28 = 0;
        data38 = 0;
        data3c = 0;
        m_player = 0;
        data00 = 1.0f;
        data04 = 1.0f;
        data08 = 1.0f;
        data0c = 0.0f;
        data10 = 0.0f;
        data14 = 0.0f;
        data18 = 0.0f;
        data20 = 0.0f;
        data1c = 0.0f;
        data24 = 0;
        data44 = 0;
    }
    ~OPTION_PARSING_PLAYER() {
        if (m_player) {
            delete m_player;
            m_player = 0;
        }
    }
    bool Load(const char* name) {
        if (!FILE_exists(name)) {
            PRINT_string("Couldn't find movie file %s\n", name);
            return false;
        }
        m_player = new RCMP::AV_PLAYER(name, 0x100000, 0, 0, (RCMP::AV_PLAYER::LOAD_ENUM)0, (RCMP::AV_PLAYER::SOUND_ENUM)0);
        return true;
    }
    void Play(bool);

    float data00;
    float data04;
    float data08;
    float data0c;
    float data10;
    float data14;
    float data18;
    float data1c;
    float data20;
    unsigned char data24;
    bool m_playing;
    unsigned char unknown26[2];
    int data28;
    unsigned char data2c;
    unsigned char data2d;
    unsigned char data2e;
    unsigned char data2f;
    unsigned char data30;
    unsigned char data31;
    unsigned char data32;
    unsigned char data33;
    unsigned char data34;
    unsigned char unknown35[3];
    int data38;
    int data3c;
    RCMP::AV_PLAYER* m_player;
    int data44;
};

extern OPTION_PARSING_PLAYER* CurentOPPlayer;

void RCMP_PlayMovie(char* name, bool loop) {
    CurentOPPlayer = new OPTION_PARSING_PLAYER;
    if (CurentOPPlayer->Load(name)) {
        CurentOPPlayer->m_playing = true;
        CurentOPPlayer->Play(loop);
        delete CurentOPPlayer;
        CurentOPPlayer->m_playing = false;
    }
}
