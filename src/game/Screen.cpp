// CScreen accessors: the depth and frame formats (returned by address) and
// the width and height. CScreen is named by the mangled symbols; the members
// and the format records are inferred from offsets, and the result types are
// inferred. The rest of the file is not part of this unit.
struct ScreenFormatView {
    unsigned char data[4];
};

class CScreen {
public:
    ScreenFormatView* GetDepthFormat();
    ScreenFormatView* GetFrameFormat();
    int GetWidth();
    int GetHeight();

    unsigned char unknown00[32];
    int m_width;
    int m_height;
    unsigned char unknown28[8];
    ScreenFormatView m_frameFormat;
    ScreenFormatView m_depthFormat;
};

ScreenFormatView* CScreen::GetDepthFormat() {
    return &m_depthFormat;
}

ScreenFormatView* CScreen::GetFrameFormat() {
    return &m_frameFormat;
}

int CScreen::GetWidth() {
    return m_width;
}

int CScreen::GetHeight() {
    return m_height;
}
