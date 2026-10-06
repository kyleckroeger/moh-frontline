// RCMP::AUDIO_PLAYER: the movie player's audio, a tap-mode sound stream fed
// from a buffer allocated through the RCMP system hook. RCMP, AUDIO_PLAYER,
// RCMP_SYSTEM and SNDPLAYOPTS are named by the mangled symbols; the members,
// the system hook's parameters (name, size, two zeros and the system's user
// value) and the status arrays (as in sststat.c and sstreqst.c) are inferred.
struct SNDPLAYOPTS {
    unsigned char data[24];
};

extern "C" {
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int SNDSYS_inited(void);
void DEBUG_break(void);
int SNDplaysetdef(SNDPLAYOPTS*);
int SNDSTRM_overheadtap(int, int);
int SNDSTRM_createtap(int, SNDPLAYOPTS*, int, int, void*, int);
int SNDSTRM_queuerequestid(int, int, int);
int SNDSTRM_status(int, int*);
int SNDSTRM_requeststatus(int, unsigned int*);
int SNDSTRM_modifyhold(int, int);
int SNDSTRM_pitchmult(int, unsigned int);
int SNDSTRM_destroy(int);
void MEM_free(void*);
}

namespace RCMP {

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int);
    void (*m_free)(void*);
    int m_userValue;
};

extern RCMP_SYSTEM rcmp_sys;

class AUDIO_PLAYER {
public:
    AUDIO_PLAYER(int, int);
    ~AUDIO_PLAYER();
    void operator delete(void* p) { rcmp_sys.m_free(p); }
    bool IsAudioFinished();
    int StartSound();
    int SetSpeed(unsigned int);

    float m_speed;
    void* m_buffer;
    int m_field08;
    int m_stream;
    int m_request;
};

bool AUDIO_PLAYER::IsAudioFinished() {
    unsigned int requestStatus[4];
    int status[3];

    SNDSYS_entercritical();
    SNDSTRM_status(m_stream, status);
    SNDSTRM_requeststatus(status[1], requestStatus);
    SNDSYS_leavecritical();
    return requestStatus[2] == 0;
}

int AUDIO_PLAYER::StartSound() {
    if (m_request != -1) {
        SNDSYS_entercritical();
        int result = SNDSTRM_modifyhold(m_request, 0);
        SNDSYS_leavecritical();
        return result;
    }
    return 0;
}

int AUDIO_PLAYER::SetSpeed(unsigned int speed) {
    if (m_stream != -1) {
        SNDSYS_entercritical();
        int result = SNDSTRM_pitchmult(m_stream, speed);
        SNDSYS_leavecritical();
        return result;
    }
    return 0;
}

AUDIO_PLAYER::~AUDIO_PLAYER() {
    if (m_buffer) {
        SNDSTRM_destroy(m_stream);
        MEM_free(m_buffer);
        m_buffer = 0;
    }
}

AUDIO_PLAYER::AUDIO_PLAYER(int channels, int request) {
    if (!SNDSYS_inited()) {
        DEBUG_break();
        return;
    }
    m_request = -1;
    m_buffer = 0;
    m_stream = -1;
    SNDPLAYOPTS opts;
    SNDplaysetdef(&opts);
    int size = SNDSTRM_overheadtap(1, 30);
    m_buffer = rcmp_sys.m_alloc("AV::audiobuff", size, 0, 0, rcmp_sys.m_userValue);
    m_stream = SNDSTRM_createtap(channels, &opts, 1, 30, m_buffer, size);
    m_request = SNDSTRM_queuerequestid(m_stream, -1, request);
    m_speed = 1.0f;
}

}
