// A fragment of sound.cpp; the file name is this project's. The sound lock and the CSoundStream wrappers over the EA stream API
// (pitch-multiplier pause, fades, volume, status, purge and file queueing).
// CSoundSysLock, CSoundStream, SSndStrmRequestStatus and SSndStreamStatus are
// named by the mangled symbols; the stream handle at the start of the object
// and the result types are inferred, and the status records are left
// incomplete. The rest of the file is not part of this unit.
struct SSndStrmRequestStatus;
struct SSndStreamStatus;

extern "C" {
void SNDSYS_entercritical(void);
int SNDSTRM_pitchmult(int, unsigned int);
int SNDSTRM_autovol(int, int, int);
int SNDSTRM_getprogvol(int);
int SNDSTRM_requeststatus(int, SSndStrmRequestStatus*);
int SNDSTRM_status(int, SSndStreamStatus*);
int SNDSTRM_vol(int, int);
int SNDSTRM_purge(int);
int SNDSTRM_queuefile(int, int, const char*, int);
}

class CSoundSysLock {
public:
    CSoundSysLock();
};

class CSoundStream {
public:
    void Unpause();
    void Pause();
    void FadeVolume(int, int);
    int GetVolume() const;
    int GetRequestStatus(int, SSndStrmRequestStatus&) const;
    int GetStatus(SSndStreamStatus&) const;
    void SetVolume(int);
    void PurgeQueue();
    int QueueFile(const char*, int, int);

    int m_stream;
};

CSoundSysLock::CSoundSysLock() {
    SNDSYS_entercritical();
}

void CSoundStream::Unpause() {
    SNDSTRM_pitchmult(m_stream, 4096);
}

void CSoundStream::Pause() {
    SNDSTRM_pitchmult(m_stream, 0);
}

void CSoundStream::FadeVolume(int time, int volume) {
    SNDSTRM_autovol(m_stream, time, volume);
}

int CSoundStream::GetVolume() const {
    return SNDSTRM_getprogvol(m_stream);
}

int CSoundStream::GetRequestStatus(int request, SSndStrmRequestStatus& status) const {
    return SNDSTRM_requeststatus(request, &status);
}

int CSoundStream::GetStatus(SSndStreamStatus& status) const {
    return SNDSTRM_status(m_stream, &status);
}

void CSoundStream::SetVolume(int volume) {
    SNDSTRM_vol(m_stream, volume);
}

void CSoundStream::PurgeQueue() {
    SNDSTRM_purge(m_stream);
}

int CSoundStream::QueueFile(const char* name, int offset, int flags) {
    return SNDSTRM_queuefile(m_stream, offset, name, flags);
}
