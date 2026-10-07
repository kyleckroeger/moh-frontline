// The weak offsetPtr<T> instantiations at 0x80044c1c (no file record names
// them; they follow propdat.cpp's functions): each rebases a non-null pointer
// read from a loaded file by adding the file's base address. The template and
// its instantiations are named by the mangled symbols (the same template as
// in soundtable.cpp); the explicit instantiations reproduce the order of the
// weak copies. BPDPolyPath and the other record types are named by the
// symbols and left incomplete.
struct BPDPolyPath;
struct BPDParticleTweaker;
struct BPDLight;
struct PropPlane4;
struct BPDLightVolume;
struct MOH_animatedLight_Struct;
struct MOH_particle_Struct;

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

template void offsetPtr<BPDPolyPath>(BPDPolyPath*&, int);
template void offsetPtr<BPDParticleTweaker>(BPDParticleTweaker*&, int);
template void offsetPtr<BPDLight>(BPDLight*&, int);
template void offsetPtr<PropPlane4>(PropPlane4*&, int);
template void offsetPtr<BPDLightVolume>(BPDLightVolume*&, int);
template void offsetPtr<float>(float*&, int);
template void offsetPtr<unsigned long>(unsigned long*&, int);
template void offsetPtr<MOH_animatedLight_Struct>(MOH_animatedLight_Struct*&, int);
template void offsetPtr<char>(char*&, int);
template void offsetPtr<MOH_particle_Struct>(MOH_particle_Struct*&, int);
template void offsetPtr<void*>(void**&, int);
template void offsetPtr<void>(void*&, int);
