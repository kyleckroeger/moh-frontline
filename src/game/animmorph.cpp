// The static initialisation of animmorph.cpp (0x80065684): the morph
// animation matrix cache (constructed with 16, 384 and 8, as the skinned one
// in animskinned.cpp; the meaning of the arguments is not known). The rest of
// the file is not reconstructed. CAnimMatrixCache and g_animMorphCache are
// named by the symbols; the object size comes from the symbol.
class CAnimMatrixCache {
public:
    CAnimMatrixCache(int, int, int);

    unsigned char unknown00[36];
};

CAnimMatrixCache g_animMorphCache(16, 384, 8);
