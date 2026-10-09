// The static initialisation of player_weapon_object.cpp (0x800a8db8): the
// camera-to-clip matrix's inline constructor initialises the matrix class
// once. The rest of the file is in other units. g_Cam2clip and CMatrix are
// named by the symbols; the object size comes from the symbol.
class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

static CMatrix g_Cam2clip;
