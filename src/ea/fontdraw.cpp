// The 8-bit text entry points: FONT_drawtexta and FONT_getrecta forward to the
// templated drawers for unsigned char text (FONT_drawtexta with no user data).
// FONT and the template names come from the mangled symbols; the result types
// are inferred. The templated drawers and the rest of the file are not part of
// this unit.
struct FONT;

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
