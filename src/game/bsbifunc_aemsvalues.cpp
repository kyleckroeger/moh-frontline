// A fragment of bsbifunc.cpp (0x8001e288): GetAEMSStructureValues, which fills
// four values for an AEMS structure type (types 1, 3, 4, 6, 7, 8, 14, 15 and 16
// through a jump table; other types leave them unchanged). The file name is
// this project's; the original record is bsbifunc.cpp and the functions around
// it are not reconstructed. The function is named by the mangled symbol; the
// meaning of the four values and the parameter names are not known.
void GetAEMSStructureValues(int type, int* first, int* second, int* third, int* fourth) {
    switch (type) {
    case 1:
        *first = 3;
        *second = 5;
        *third = -1;
        *fourth = 32;
        break;
    case 3:
        *first = 3;
        *second = 2;
        *third = -1;
        *fourth = 20;
        break;
    case 4:
        *first = 4;
        *second = 5;
        *third = -1;
        *fourth = 28;
        break;
    case 6:
        *first = 2;
        *second = 3;
        *third = -1;
        *fourth = 16;
        break;
    case 7:
        *first = 4;
        *second = 5;
        *third = -1;
        *fourth = 40;
        break;
    case 8:
        *first = 3;
        *second = 4;
        *third = -1;
        *fourth = 24;
        break;
    case 14:
        *first = 4;
        *second = 5;
        *third = -1;
        *fourth = 40;
        break;
    case 15:
        *first = 3;
        *second = 4;
        *third = -1;
        *fourth = 24;
        break;
    case 16:
        *first = 3;
        *second = 2;
        *third = 1;
        *fourth = 28;
        break;
    }
}
