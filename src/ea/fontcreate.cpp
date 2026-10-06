// FONT_create/FONT_destroy: forward to the current font driver's hooks. The
// driver record's layout is inferred from offsets.
struct FONTDRIVER {
    void* field0;
    void* field4;
    void* field8;
    void (*create)(void*);
    void (*destroy)(void*);
};

extern "C" {
extern FONTDRIVER* FONTcurrentdriver;

void* FONT_create(void* font) {
    if (FONTcurrentdriver->create)
        FONTcurrentdriver->create(font);
    return font;
}

void FONT_destroy(void* font) {
    if (FONTcurrentdriver->destroy)
        FONTcurrentdriver->destroy(font);
}
}
