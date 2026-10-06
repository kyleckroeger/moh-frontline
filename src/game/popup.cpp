// PopUpMessage: the accessors in the middle of the file (the getters,
// SetString and the setters, from GetString to Set_placement). PopUpMessage is
// named by the mangled symbols; the members are inferred from the accessors'
// offsets and their names follow the accessor names. The file is compiled with
// -inline deferred,auto, so the functions are listed in reverse image order.
// The scrolling functions before them and Reset, the destructor and the
// constructor after them are not part of this unit (draft of the whole file
// in scratch/lib/popup_wip.cpp; Scroll is off and the small data of the tail
// cannot be placed on its own).
extern "C" {
char* strncpy(char*, const char*, unsigned long);
}

class PopUpMessage {
public:
    PopUpMessage();
    ~PopUpMessage();
    void Reset();
    void Set_placement(unsigned char);
    void Set_screenLocs(unsigned short, unsigned short);
    void Set_property(unsigned char);
    void Set_onScreenTime(float);
    void Set_textSize(unsigned char);
    void Set_rgba(unsigned char, unsigned char, unsigned char, unsigned char);
    void Set_top(unsigned short);
    void Set_bottom(unsigned short);
    void ChangeAvailability(unsigned char);
    void SetString(const char*);
    unsigned char Available();
    unsigned char Get_placement();
    unsigned short Get_screenLocX();
    unsigned short Get_screenLocY();
    unsigned char Get_property();
    float Get_onScreenTime();
    unsigned char Get_r();
    unsigned char Get_g();
    unsigned char Get_b();
    unsigned char Get_a();
    unsigned short Get_top();
    char* GetString();
    void MoveTop();
    void Scroll();
    void UpdateDisplayTime(float);

    unsigned char placement;
    unsigned short screenLocX;
    unsigned short screenLocY;
    unsigned char property;
    float onScreenTime;
    unsigned short textSize;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    unsigned short top;
    unsigned short bottom;
    unsigned short field16;
    unsigned char available;
    char string[50];
};

void PopUpMessage::Set_placement(unsigned char value) {
    placement = value;
}

void PopUpMessage::Set_screenLocs(unsigned short x, unsigned short y) {
    screenLocX = x;
    screenLocY = y;
}

void PopUpMessage::Set_property(unsigned char value) {
    property = value;
}

void PopUpMessage::Set_onScreenTime(float value) {
    onScreenTime = value;
}

void PopUpMessage::Set_textSize(unsigned char value) {
    textSize = value;
}

void PopUpMessage::Set_rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

void PopUpMessage::Set_top(unsigned short value) {
    top = value;
}

void PopUpMessage::Set_bottom(unsigned short value) {
    bottom = value;
}

void PopUpMessage::ChangeAvailability(unsigned char value) {
    available = value;
}

void PopUpMessage::SetString(const char* text) {
    strncpy(string, text, 50);
}

unsigned char PopUpMessage::Available() {
    return available;
}

unsigned char PopUpMessage::Get_placement() {
    return placement;
}

unsigned short PopUpMessage::Get_screenLocX() {
    return screenLocX;
}

unsigned short PopUpMessage::Get_screenLocY() {
    return screenLocY;
}

unsigned char PopUpMessage::Get_property() {
    return property;
}

float PopUpMessage::Get_onScreenTime() {
    return onScreenTime;
}

unsigned char PopUpMessage::Get_r() {
    return r;
}

unsigned char PopUpMessage::Get_g() {
    return g;
}

unsigned char PopUpMessage::Get_b() {
    return b;
}

unsigned char PopUpMessage::Get_a() {
    return a;
}

unsigned short PopUpMessage::Get_top() {
    return top;
}

char* PopUpMessage::GetString() {
    return string;
}
