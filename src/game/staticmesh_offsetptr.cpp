// A fragment of staticmesh.cpp (0x800f32a0): the weak offsetPtr<T>
// instantiations for the static mesh file's records, each rebasing a non-null
// pointer by the file base. The template and its instantiations are named by
// the mangled symbols (as in propdat_offsetptr.cpp); the explicit
// instantiations reproduce the order of the weak copies and the record types
// are left incomplete. The rest of the file is not part of this unit.
struct StaticMesh;
struct MSHVertex_GC;
struct MSHAttachNode;
struct MSHNode;
struct MSHChunk;
class CMSHMatBin;

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

template void offsetPtr<StaticMesh>(StaticMesh*&, int);
template void offsetPtr<StaticMesh*>(StaticMesh**&, int);
template void offsetPtr<MSHVertex_GC>(MSHVertex_GC*&, int);
template void offsetPtr<unsigned char>(unsigned char*&, int);
template void offsetPtr<MSHAttachNode>(MSHAttachNode*&, int);
template void offsetPtr<MSHNode>(MSHNode*&, int);
template void offsetPtr<MSHChunk>(MSHChunk*&, int);
template void offsetPtr<CMSHMatBin>(CMSHMatBin*&, int);
