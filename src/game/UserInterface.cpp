// UserInterface's destructor: outside multiplayer it deletes the HUD sprites
// and the bullet-count font. UserInterface, CSprite, CFont and CRenderBin are
// named by the mangled symbols; the sprite and font pointers are the class's
// static members (defined with the rest of the file, not part of this unit).
// CRenderBin is an inferred view: 28 bytes of members, then its virtual table
// pointer, with the virtual destructor in the first slot.
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class CSprite : public CRenderBin {
public:
    virtual ~CSprite();
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
};

extern bool g_bInMultiplayerMode;

class UserInterface {
public:
    ~UserInterface();

    static CSprite* healthSprite;
    static CSprite* compassSprite;
    static CSprite* compassEdgeSprite;
    static CSprite* hitMeterSprite;
    static CSprite* blackSprite;
    static CFont* bulletText;
};

UserInterface::~UserInterface() {
    if (!g_bInMultiplayerMode) {
        delete healthSprite;
        delete compassSprite;
        delete hitMeterSprite;
        delete compassEdgeSprite;
        delete blackSprite;
        delete bulletText;
    }
}
