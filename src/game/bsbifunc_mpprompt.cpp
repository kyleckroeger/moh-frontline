// A fragment of bsbifunc.cpp (0x80023048): GetMPWeaponPrompt, which maps a
// script bullet type (0 to 25) to the text id of its multiplayer weapon prompt
// (-1 for other types) through a jump table. The cases are written in the order
// of their bodies in the original. The file name is this project's; the
// original record is bsbifunc.cpp and the functions around it are not
// reconstructed. The function and EScriptBulletType are named by the mangled
// symbol; the enumerator names are unknown (the cases are written as numbers)
// and the int return type is inferred.
enum EScriptBulletType {};

int GetMPWeaponPrompt(EScriptBulletType type) {
    switch (type) {
    case 7:
        return 407;
    case 0:
        return 408;
    case 4:
        return 409;
    case 8:
        return 410;
    case 9:
        return 411;
    case 10:
        return 412;
    case 11:
        return 415;
    case 12:
        return 416;
    case 5:
        return 417;
    case 13:
        return 418;
    case 14:
        return 419;
    case 15:
        return 420;
    case 16:
        return 423;
    case 17:
        return 424;
    case 6:
        return 425;
    case 18:
        return 426;
    case 19:
        return 429;
    case 20:
        return 432;
    case 2:
        return 433;
    case 1:
        return 435;
    case 21:
        return 437;
    case 22:
        return 438;
    case 23:
        return 439;
    case 3:
        return 440;
    case 24:
        return 441;
    case 25:
        return 767;
    }
    return -1;
}
