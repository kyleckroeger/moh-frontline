// CAnimated's weak defaults: no rotation or translation to extract, an empty
// rotation setter and a null morph cast. The names come from the mangled
// symbols; the result types are inferred and the class is a non-virtual view.
// The defaults are inline in the original (weak symbols), so they are defined
// __declspec(weak). The rest of the file is not part of this unit.
class CVector3;
class CAnimMorph;

class CAnimated {
public:
    int ExtractRotation(long*);
    int ExtractTranslation(CVector3*);
    void SetRotation(long);
    CAnimMorph* AsAnimMorph();
};

__declspec(weak) int CAnimated::ExtractRotation(long*) {
    return 0;
}

__declspec(weak) int CAnimated::ExtractTranslation(CVector3*) {
    return 0;
}

__declspec(weak) void CAnimated::SetRotation(long) {
}

__declspec(weak) CAnimMorph* CAnimated::AsAnimMorph() {
    return 0;
}
