// The end of decal.cpp (0x80083ec0): the static initialisation of the file's
// decal list (512 bullet decals built through their constructor by
// __construct_array) and of its identity matrix (the inline constructor
// initialises the matrix class once, then the identity is set), followed by
// the weak BulletDecal constructor emitted after it, which sets the colour of
// each of the decal's four vertices to (0, 0, 0, 128). The rest of the file is
// in other units; the weak this-adjusting thunks after the constructor are not
// part of this unit. BulletDecal, g_DecalList and g_Ident are named by the
// symbols; the object sizes come from the symbols, the vertex layout, its colour
// view and the identity-matrix constructor are inferred.
/* Inferred: a plain colour (the vertex is constructor-less). */
struct ColorView {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    void Set(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) {
        r = ar;
        g = ag;
        b = ab;
        a = aa;
    }
};

class CMatrix {
public:
    /* Inferred: a constructor that also sets the identity. */
    CMatrix(bool identity) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    static void InitClass();
    void Ident();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

/* Inferred: a decal vertex (position, colour, texture coordinates). */
struct DecalVertexView {
    float position[3];
    ColorView color;
    float uv[2];
};

class BulletDecal {
public:
    BulletDecal() {
        m_verts[0].color.Set(0, 0, 0, 128);
        m_verts[1].color.Set(0, 0, 0, 128);
        m_verts[2].color.Set(0, 0, 0, 128);
        m_verts[3].color.Set(0, 0, 0, 128);
    }

    DecalVertexView m_verts[4];
    unsigned char unknown60[8];
};

static BulletDecal g_DecalList[512];
static CMatrix g_Ident(true);
