// A fragment of rcmpbase.cpp (the movie player's base), the start of the file
// record (0x80105258): the CODEC_IDATA constructor (streamer, chunk get and
// release callbacks, a value), the CHUNK constructor, and DECODER::ReleaseChunk
// and GetChunk (a pending chunk first, otherwise the get callback) and
// ReleaseFrame (forwarded to the chosen codec), then the frame queries
// forwarded to the chosen codec (GetFrame first fetches a pending chunk while
// no codec is chosen; without a codec they return null, 0.0f and 0). The file
// name is this project's; the original record is rcmpbase.cpp (rcmpbase.cpp
// holds the decoder's codec choice and destructor, which follow). RCMP, DECODER, CODEC, CHUNK, CODEC_IDATA, STREAMER and FRAME
// are named by the mangled symbols and RTTI; members and the codec's virtual
// function names are inferred. The global RCMP_SYSTEM is declared extern.
// RCMP (the movie player) base: the decoder that feeds stream chunks to the
// chosen codec and forwards frame requests to it, and the global
// RCMP_SYSTEM with its allocation hooks. RCMP, DECODER, CODEC, CHUNK,
// CODEC_IDATA, RCMP_SYSTEM, STREAMER and FRAME are named by the mangled
// symbols and RTTI; members and the codec's virtual function names are
// inferred from the calls and are not original.
namespace RCMP {

class DECODER;
class STREAMER;
class FRAME;

class CHUNK {
public:
    CHUNK();

    void* m_data;
    void* m_next;
    unsigned int m_size;
};

typedef void (*GETCHUNKFUNC)(DECODER*, STREAMER*, CHUNK**);
typedef void (*RELEASECHUNKFUNC)(DECODER*, STREAMER*, CHUNK*);

class CODEC_IDATA {
public:
    CODEC_IDATA() {
        m_value = 2;
        m_streamer = 0;
        m_getChunk = 0;
        m_releaseChunk = 0;
    }
    CODEC_IDATA(STREAMER*, GETCHUNKFUNC, RELEASECHUNKFUNC, unsigned int);

    STREAMER* m_streamer;
    GETCHUNKFUNC m_getChunk;
    RELEASECHUNKFUNC m_releaseChunk;
    unsigned int m_value;
};

class CODEC {
public:
    virtual ~CODEC();
    virtual void Init(DECODER*);
    virtual FRAME* GetFrame(unsigned int);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(FRAME*);
};

class RCMP_SYSTEM {
public:
    RCMP_SYSTEM() : m_alloc(0), m_free(0) {}
    virtual ~RCMP_SYSTEM() {}

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
    int m_field0C;
};

extern RCMP_SYSTEM rcmp_sys;

class DECODER {
public:
    DECODER(const CODEC_IDATA*);
    virtual ~DECODER();
    static void operator delete(void* p) { rcmp_sys.m_free(p); }
    void ChooseCodec(CODEC*, CHUNK*);
    void FreeChosenCodec();
    unsigned int GetCurrentFrameNumber();
    float GetFrameRate();
    FRAME* GetFrame(unsigned int);
    void ReleaseFrame(FRAME*);
    CHUNK* GetChunk();
    void ReleaseChunk(CHUNK*);

    CHUNK* m_pending;
    CODEC_IDATA m_idata;
    CODEC* m_codec;
};

CODEC_IDATA::CODEC_IDATA(STREAMER* streamer, GETCHUNKFUNC getChunk, RELEASECHUNKFUNC releaseChunk,
                         unsigned int value) {
    m_value = value;
    m_streamer = streamer;
    m_getChunk = getChunk;
    m_releaseChunk = releaseChunk;
}

CHUNK::CHUNK() {
    m_next = 0;
    m_size = 0;
    m_data = 0;
}

void DECODER::ReleaseChunk(CHUNK* chunk) {
    m_idata.m_releaseChunk(this, m_idata.m_streamer, chunk);
}

CHUNK* DECODER::GetChunk() {
    CHUNK* chunk;

    if (m_pending) {
        chunk = m_pending;
        m_pending = 0;
        return chunk;
    }
    m_idata.m_getChunk(this, m_idata.m_streamer, &chunk);
    return chunk;
}

void DECODER::ReleaseFrame(FRAME* frame) {
    m_codec->ReleaseFrame(frame);
}

FRAME* DECODER::GetFrame(unsigned int frame) {
    if (!m_codec && !m_pending)
        m_idata.m_getChunk(this, m_idata.m_streamer, &m_pending);
    return !m_codec ? 0 : m_codec->GetFrame(frame);
}

float DECODER::GetFrameRate() {
    return !m_codec ? 0.0f : m_codec->GetFrameRate();
}

unsigned int DECODER::GetCurrentFrameNumber() {
    return !m_codec ? 0 : m_codec->GetCurrentFrameNumber();
}

void DECODER::FreeChosenCodec();

void DECODER::ChooseCodec(CODEC* codec, CHUNK* chunk);

DECODER::~DECODER();

DECODER::DECODER(const CODEC_IDATA* data);

}


