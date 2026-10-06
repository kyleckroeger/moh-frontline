// Screen fades: CFaderControl, a colour pair with a duration and a virtual
// Update, and its fading test. CFaderControl and CColor are named by the
// mangled symbols and RTTI; members are inferred from offsets. This unit
// covers IsFading and the constructor; the fader bin before them and the
// CFader functions after them are not part of it (draft in scratch).
struct CColor {
    CColor() {}
    CColor(const CColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);
    bool IsFading() const;

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

bool CFaderControl::IsFading() const {
    return m_time <= m_duration;
}

CFaderControl::CFaderControl(CColor from, CColor to, float duration)
    : m_from(from), m_to(to), m_duration(duration), m_time(0.0f) {
}
