// GCSW_VD, the software video display: completing a draw releases the frame to
// the decoder, redrawing waits for the retrace and converts the frame's Y, U
// and V planes (stored after the header or at an offset it gives) into the
// display buffer at the position, Draw stores the frame and redraws, the
// scale setter is empty and the position setter stores integer coordinates.
// GCSW_VD, RCMP::FRAME and RCMP::DECODER are named by the mangled symbols;
// the virtual functions are declared in the order of __vt__7GCSW_VD; the
// members and the frame-header and display-target views are inferred from
// offsets. The rest of the file is not part of this unit.

/* inferred view of the decoded frame's header */
struct FrameHeaderView {
    unsigned char unknown00[4];
    short width;
    short height;
    unsigned char unknown08[4];
    unsigned char unknown0c_0 : 3;
    unsigned char external : 1;
    unsigned char unknown0c_1 : 4;
    unsigned char unknown0d[3];
    int dataOffset;
};

namespace RCMP {
class FRAME {
public:
    unsigned char unknown00[4];
    FrameHeaderView* header;
};

class DECODER {
public:
    void ReleaseFrame(FRAME*);
};
}

/* inferred view of the display target */
struct DisplayTargetView {
    unsigned char unknown00[4];
    unsigned short width;
    unsigned short height;
};

extern "C" void VIWaitForRetrace(void);
void YUV_convert(unsigned char**, int, int, int, int, int, void*, int, int, int);

class GCSW_VD {
public:
    virtual ~GCSW_VD();
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    RCMP::DECODER* m_decoder;
    RCMP::FRAME* m_frame;
    DisplayTargetView* m_target;
    void* m_buffer;
    int m_x;
    int m_y;
};

void GCSW_VD::CompleteDraw() {
    m_decoder->ReleaseFrame(m_frame);
    m_frame = 0;
}

void GCSW_VD::ReDraw() {
    unsigned short stride = (m_target->width + 15) & ~15;
    FrameHeaderView* header = m_frame->header;
    void* buffer = m_buffer;
    unsigned char* data;
    unsigned char* planes[3];
    int width;
    int height;

    if (header->external)
        data = (unsigned char*)header + header->dataOffset;
    else
        data = (unsigned char*)header + 16;
    width = header->width;
    height = header->height;
    planes[0] = data;
    planes[1] = data + width * height;
    planes[2] = planes[1] + (width / 2) * (height / 2);
    VIWaitForRetrace();
    YUV_convert(planes, m_x, m_y, width, width, height, buffer, m_target->width, m_target->height / 2, stride);
}

void GCSW_VD::Draw(RCMP::FRAME* frame) {
    m_frame = frame;
    ReDraw();
}

void GCSW_VD::SetScale(float, float) {
}

void GCSW_VD::SetPosition(float x, float y, float) {
    m_x = x;
    m_y = y;
}
