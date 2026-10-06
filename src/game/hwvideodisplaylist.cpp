// GCHW_VD, the hardware video display: setting the scale or the position
// recomputes the drawn extent as size * scale + position on each axis (the
// position setter also stores the depth). GCHW_VD is named by the mangled
// symbols; the members are inferred from offsets and the class is a
// non-virtual view. The rest of the file is not part of this unit.
class GCHW_VD {
public:
    void SetScale(float, float);
    void SetPosition(float, float, float);

    unsigned char unknown00[112];
    float m_width;
    float m_height;
    float m_z;
    float m_x;
    float m_y;
    float m_right;
    float m_bottom;
    float m_scaleX;
    float m_scaleY;
};

void GCHW_VD::SetScale(float x, float y) {
    m_scaleX = x;
    m_scaleY = y;
    m_right = m_width * m_scaleX + m_x;
    m_bottom = m_height * m_scaleY + m_y;
}

void GCHW_VD::SetPosition(float x, float y, float z) {
    m_x = x;
    m_y = y;
    m_right = m_width * m_scaleX + m_x;
    m_bottom = m_height * m_scaleY + m_y;
    m_z = z;
}
