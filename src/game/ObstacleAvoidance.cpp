// The static initialisation of ObstacleAvoidance.cpp (0x800547d0): the 13
// obstacle-avoidance line vectors are built through CVector3's constructor
// (__construct_array). The rest of the file is not reconstructed.
// f_rv3ObstacleAvoidanceLines and CVector3 are named by the symbols; the
// array size comes from the symbol and the vector layout is inferred. The
// vector's inline constructor, emitted for the array construction, follows
// the static initialisation (a weak copy of this file).
class CVector3 {
public:
    CVector3() {}

    float x;
    float y;
    float z;
    float w;
};

CVector3 f_rv3ObstacleAvoidanceLines[13];
