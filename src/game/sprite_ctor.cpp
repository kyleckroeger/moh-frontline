// A fragment of sprite.cpp (0x80082648): the CSprite constructor. The bin is
// built through the inline CRenderBinData and CRenderBin constructors, takes
// the requested render priority and starts without a file; when a name is
// given, the file is loaded from the big file chosen by the load type
// (briefing log, shell, slides, else the level). A failed load or a file
// without the "SHPG" shape tag is reported and leaves the sprite empty;
// otherwise the sprite keeps the file, registers with the render list and
// sets its texture from the shape file. The file name is this project's; the
// original record is sprite.cpp. The classes, functions and globals are named
// by the mangled symbols; the members, the load-type values and the base
// layout are inferred, and the messages are entries of the file's .rodata
// string pool. CSprite declares Render (defined elsewhere) first so its
// global table stays elsewhere.
class CDmaPacket;
class CDmaTag;
struct ShapeFile;

void* TLT_LoadFileFromShellBriefingLogBigFile(const char*, int*);
void* TLT_LoadFileFromShellBigFile(const char*, int*);
void* TLT_LoadFileFromSlidesBigFile(const char*, int*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void DebugMsg(const char*, ...);

enum ERenderPriority {};

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

extern CRenderList* g_pRenderList;

class CTexture {
public:
    void Set(ShapeFile*, bool);

    unsigned char unknown00[64];
};

class CSprite : public CRenderBin {
public:
    enum ESpriteLoadType {};

    virtual int Render(CDmaPacket&, void*);
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();

    void* m_file;
    CTexture m_texture;
};

CSprite::CSprite(const char* name, ESpriteLoadType type, ERenderPriority priority) {
    m_priority = priority;
    m_file = 0;
    if (!name)
        return;

    int size;
    char* data;
    if (type == 3)
        data = (char*)TLT_LoadFileFromShellBriefingLogBigFile(name, &size);
    else if (type == 2)
        data = (char*)TLT_LoadFileFromShellBigFile(name, &size);
    else if (type == 4)
        data = (char*)TLT_LoadFileFromSlidesBigFile(name, &size);
    else
        data = (char*)TLT_LoadFileFromLevelBigFile(name, &size);

    if (!data) {
        DebugMsg("CSprite::CSprite(%s):  TLT_LoadFile... FAILED!!!\n", name);
        return;
    }
    if (data[0] != 'S' || data[1] != 'H' || data[2] != 'P' || data[3] != 'G') {
        DebugMsg("CCSprite::CCSprite():  File is not a GAMECUBE Shape, type = %s\n", data);
        return;
    }
    m_file = data;
    ShapeFile* shape = (ShapeFile*)m_file;
    g_pRenderList->Register(*this);
    m_texture.Set(shape, false);
}
