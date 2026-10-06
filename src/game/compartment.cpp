// CCompartment's visibility and draw-enabled queries and the shared shadow
// texture setter. CCompartment, CTexture and g_CptShadowBin are named by the
// symbols; the members and the shadow-bin view are inferred from offsets, and
// the class is a non-virtual view. The rest of the file is not part of this
// unit.
class CDrawContext;
class CTexture;

struct CptShadowBinView {
    unsigned char unknown00[32];
    CTexture* texture;
};

extern CptShadowBinView g_CptShadowBin;

class CCompartment {
public:
    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;
    static void SetShadowTexture(CTexture&);

    unsigned char unknown00[96];
    bool m_drawEnabled;
};

bool CCompartment::IsVisible(CDrawContext&) const {
    return true;
}

bool CCompartment::IsDrawEnabled() const {
    return m_drawEnabled;
}

void CCompartment::SetShadowTexture(CTexture& texture) {
    g_CptShadowBin.texture = &texture;
}
