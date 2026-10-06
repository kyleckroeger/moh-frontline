// The null font driver: a draw entry that does nothing.
struct FONT;
struct FONTCHAR;

// Only the size (20 bytes) and the draw entry at +0 are established.
struct FONTDRIVER {
    void (*draw)(FONT*, const FONTCHAR*, float, float);
    void* unknown04[4];
};

static void NULL_draw(FONT*, const FONTCHAR*, float, float) {
}

FONTDRIVER FONTnulldriver = { NULL_draw };
