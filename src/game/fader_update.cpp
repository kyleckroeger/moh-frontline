// A fragment of fader.cpp (0x8007bfcc): CFader::Update advances the current
// fade control through its virtual Update and drops the control once its time
// has passed its duration (IsFading, defined earlier in the file, is inlined). CFader and CFaderControl are named by the mangled
// symbols; the members are inferred from offsets (see fader_color.cpp) and
// the file's globals are extern. The rest of the file is not part of this
// unit.
struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFaderControl {
public:
    virtual void Update(float);
    bool IsFading() const;

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CFader {
public:
    static void Update(float);
};

extern CFaderControl* g_pFaderControl;

// Defined in fader.cpp; repeated inline so this fragment inlines it as the
// original's call does.
inline bool CFaderControl::IsFading() const {
    return m_time <= m_duration;
}

void CFader::Update(float elapsed) {
    if (g_pFaderControl) {
        g_pFaderControl->Update(elapsed);
        if (!g_pFaderControl->IsFading())
            g_pFaderControl = 0;
    }
}
