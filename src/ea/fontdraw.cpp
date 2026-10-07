// FONT_switchcase, a file-local helper that gives the other case of an ASCII
// or Latin-1 letter (0 for any other character), and the 8-bit text entry
// points: FONT_drawtexta and FONT_getrecta forward to the templated drawers
// for unsigned char text (FONT_drawtexta with no user data). FONT and the
// function and template names come from the mangled symbols; the result
// types are inferred. The templated drawers and the rest of the file are not part of
// this unit.
struct FONT;

static int FONT_switchcase(int ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ch + 32;
    if (ch >= 'a' && ch <= 'z')
        return ch - 32;
    if (ch >= 0xc0 && ch <= 0xd6)
        return ch + 32;
    if (ch >= 0xd8 && ch <= 0xde)
        return ch + 32;
    if (ch >= 0xe0 && ch <= 0xf6)
        return ch - 32;
    if (ch >= 0xf8 && ch <= 0xfe)
        return ch - 32;
    return 0;
}

template <class T> int FONT_drawtextx(FONT*, float, float, const T*, void*);
template <class T> int FONT_getrectx(FONT*, const T*, float*, float*, float*, float*);

extern "C" {
int FONT_drawtexta(FONT* font, float x, float y, const unsigned char* text) {
    return FONT_drawtextx<unsigned char>(font, x, y, text, 0);
}

int FONT_getrecta(FONT* font, const unsigned char* text, float* x, float* y, float* w, float* h) {
    return FONT_getrectx<unsigned char>(font, text, x, y, w, h);
}
}
