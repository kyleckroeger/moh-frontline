// The static initialisation of sound.cpp (0x800ede28): the three subtitle
// colours (white, yellow and green, half alpha) and the ambient source
// position (the origin; 0.0 is an entry of the file's .sdata2 pool). The rest
// of the file is in other units or not reconstructed. _subtitle_rgb and
// g_ambientSourcePos are named by the symbols; the colour and vector
// constructors are inferred.
struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CVector3 {
public:
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

static CColor _subtitle_rgb[3] = {CColor(255, 255, 255, 128), CColor(255, 255, 0, 128), CColor(0, 255, 0, 128)};
static CVector3 g_ambientSourcePos(0.0f, 0.0f, 0.0f);
