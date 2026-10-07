// A fragment of scene.cpp (0x800d9e24): the weak CmpNodePairs, which orders
// two node pairs by their 64-bit key (unsigned less-than). CScene, UNodePair
// and CmpNodePairs are named by the mangled symbols; the pair's key view is
// inferred. The function is a weak copy of a header inline emitted in this
// file, so it is defined __declspec(weak). The rest of the file is not part
// of this unit.
class CScene {
public:
    struct UNodePair {
        unsigned long long key;
    };
};

__declspec(weak) bool CmpNodePairs(const CScene::UNodePair& a, const CScene::UNodePair& b) {
    return a.key < b.key;
}
