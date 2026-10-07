// A fragment of bsbifunc.cpp (0x8001ff7c): the script built-in that constrains
// a rotation: given a rotation, a current angle and a minimum and maximum, it
// returns the rotation reduced so that the current angle plus it stays within
// the range (through an integer/float union). It reads its arguments below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around it
// are not reconstructed. The function and globals are named by the mangled
// symbols; the value union and built-in record views are inferred.
union BSValueView {
    int i;
    float f;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_GetConstrainedRotateAngle(int** stack, void*) {
    BSValueView value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    value.f = *(float*)(*stack - (count - 1));
    float current = *(float*)(*stack - (count - 2));
    float minimum = *(float*)(*stack - (count - 3));
    float maximum = *(float*)(*stack - (count - 4));
    if (current + value.f > maximum)
        value.f = maximum - current;
    else if (current + value.f < minimum)
        value.f = minimum - current;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}
