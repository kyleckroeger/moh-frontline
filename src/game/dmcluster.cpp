// The deformable-mesh cluster's set-up and shutdown: a matrix stack of 16
// entries is created at start-up and deleted at shutdown. CMatrixStack and
// _dmeshStack are named by the symbols; the stack is 12 bytes (from the
// allocation). The scratch-matrix array's static initialiser after these
// functions builds CMatrix objects through the weak __defctor__7CMatrixFv,
// which this unit would emit a second copy of, so it is not part of the unit.
class CMatrixStack {
public:
    CMatrixStack();
    ~CMatrixStack();
    void Allocate(unsigned long);

    unsigned char unknown00[12];
};

static CMatrixStack* _dmeshStack;

void DMClusterShutdown() {
    delete _dmeshStack;
}

void DMClusterInit() {
    _dmeshStack = new CMatrixStack;
    _dmeshStack->Allocate(16);
}
