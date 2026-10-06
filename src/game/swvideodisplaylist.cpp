// GCSW_VD, the software video display: Draw stores the frame and redraws, the
// scale setter is empty and the position setter stores integer coordinates.
// GCSW_VD and RCMP::FRAME are named by the mangled symbols; the virtual
// functions are declared in the order of __vt__7GCSW_VD and the members are
// inferred from offsets. The rest of the file is not part of this unit.
namespace RCMP {
class FRAME;
}

class GCSW_VD {
public:
    virtual ~GCSW_VD();
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    unsigned char unknown04[4];
    RCMP::FRAME* m_frame;
    unsigned char unknown0c[8];
    int m_x;
    int m_y;
};

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
