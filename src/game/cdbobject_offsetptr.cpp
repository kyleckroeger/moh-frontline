// A fragment of cdbobject.cpp (0x800727d4): the weak offsetPtr<T>
// instantiations for the collision database's records, each rebasing a
// non-null pointer read from a loaded file by adding the file's base address.
// The template and its instantiations are named by the mangled symbols (the
// same template as in soundtable.cpp and propdat_offsetptr.cpp); the explicit
// instantiations reproduce the order of the weak copies, and the record types
// are left incomplete. The rest of the file is not part of this unit.
class CDBNode;
class CDBVector;
class CDBMaterial;
class CDBBranchNode;
class CDBLeafNode;
class CDBTriGroup;
class CDBTri;

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

template void offsetPtr<CDBNode>(CDBNode*&, int);
template void offsetPtr<CDBVector>(CDBVector*&, int);
template void offsetPtr<CDBMaterial>(CDBMaterial*&, int);
template void offsetPtr<CDBBranchNode>(CDBBranchNode*&, int);
template void offsetPtr<CDBLeafNode>(CDBLeafNode*&, int);
template void offsetPtr<CDBTriGroup>(CDBTriGroup*&, int);
template void offsetPtr<CDBTri>(CDBTri*&, int);
