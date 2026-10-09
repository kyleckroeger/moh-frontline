// A fragment of swvideodisplaylist.cpp (0x80108648): the GCSW_VD destructor
// (VIDEO_DRIVER's inline destructor, then the RCMP system's free as the class
// delete). RCMP::GCSW_VD_create after it is not part of this unit (draft in
// scratch). The file name is this project's; the original record is
// swvideodisplaylist.cpp. The destructor is GCSW_VD's key function, so this
// unit is compiled with RTTI on and holds the class's virtual table and the
// type information of VIDEO_DRIVER and GCSW_VD (class names are entries of
// the file's .rodata and .sdata2 pools).
// GCSW_VD, VIDEO_DRIVER, DECODER, FRAME and rcmp_sys are
// named by the mangled symbols; the members and the free hook (as in
// avplayer.cpp) are inferred, and the virtual functions are declared in table
// order.
namespace RCMP {

class FRAME;

class DECODER;

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class VIDEO_DRIVER {
public:
    virtual ~VIDEO_DRIVER() {}
    virtual void SetPosition(float, float, float) = 0;
    virtual void SetScale(float, float) = 0;
    virtual void Draw(FRAME*) = 0;
    virtual void ReDraw() = 0;
    virtual void CompleteDraw();
    static void operator delete(void* p) { rcmp_sys.m_free(p); }

    DECODER* m_decoder;
};

}

class GCSW_VD : public RCMP::VIDEO_DRIVER {
public:
    virtual ~GCSW_VD();
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    RCMP::FRAME* m_frame;
    void* m_target;
    void* m_buffer;
    int m_x;
    int m_y;
};

GCSW_VD::~GCSW_VD() {
}
