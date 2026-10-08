// The deformable-mesh cluster's set-up and shutdown: a matrix stack of 16
// entries is created at start-up and deleted at shutdown. CMatrixStack and
// _dmeshStack are named by the symbols; the stack is 12 bytes (from the
// allocation). The file's static initialiser builds the 128 matrices of the
// file-local _scratchMats array through CMatrix's array constructor wrapper
// (__defctor__7CMatrixFv: CMatrix's default constructor takes a default
// argument, unknown here); the compiler's weak copy of that wrapper is a weak
// duplicate, linked to the original copy.
class CMatrixStack {
public:
    CMatrixStack();
    ~CMatrixStack();
    void Allocate(unsigned long);

    unsigned char unknown00[12];
};

class CVector3 {
public:
    float x, y, z, w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

static CMatrixStack* _dmeshStack;

void DMClusterShutdown() {
    delete _dmeshStack;
}

void DMClusterInit() {
    _dmeshStack = new CMatrixStack;
    _dmeshStack->Allocate(16);
}

static CMatrix _scratchMats[128];
