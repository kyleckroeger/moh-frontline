// A fragment of compartment.cpp (0x80076fcc): the weak offsetPtr<T>
// instantiations for the compartment file's records (each rebases a non-null
// pointer by the file base).
// The classes, templates and functions are named by the mangled symbols; the
// fast_vec members are inferred (the layout differs from the view in
// scene.cpp, which only uses the first three words), CCompartment is a
// non-virtual view, the record types are left incomplete, and explicit
// instantiations reproduce the order of the weak copies. The rest of the file
// is not part of this unit.
struct ShapeFile;
struct CPTTexSwap;
struct gcVertex;
struct CPTNode;
struct CPTChunk;
class CCPTMatBin;

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

template void offsetPtr<ShapeFile>(ShapeFile*&, int);
template void offsetPtr<CPTTexSwap>(CPTTexSwap*&, int);
template void offsetPtr<gcVertex>(gcVertex*&, int);
template void offsetPtr<short>(short*&, int);
template void offsetPtr<signed char>(signed char*&, int);
template void offsetPtr<CPTTexSwap*>(CPTTexSwap**&, int);
template void offsetPtr<CPTNode>(CPTNode*&, int);
template void offsetPtr<CPTChunk>(CPTChunk*&, int);
template void offsetPtr<CCPTMatBin>(CCPTMatBin*&, int);
