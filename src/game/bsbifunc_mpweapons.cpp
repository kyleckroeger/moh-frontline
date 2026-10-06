// A fragment of bsbifunc.cpp (0x8001f0dc): multiplayer weapon-set built-ins.
// The weapon type and the ammo and weapon pickup prompts come from the player
// weapon table for the CRC in the given weapon-set slot; creating ammo or a
// weapon by type looks the script object's trigger CRC up in the set (the ammo
// entries 19 slots on, the weapons one slot on) and creates the object from the
// first match, returning it (or 0); creating ammo or a weapon by weapon-set id
// (1-based, checked against the set's count) sets the trigger's CRC from that
// slot (ammo 18 slots on) and creates the object. Each reads its argument below
// the script stack top, pops the built-in's arguments and writes the result to
// the new top through an integer union. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions and globals are named by the mangled symbols;
// the weapon-set, trigger and built-in record views are inferred (members at
// their offsets, names not original).
union BSValueView {
    int i;
    float f;
};

struct MPWeaponSlotView {
    int crc;
    int unknown04;
};

struct MPWeaponSetDataView {
    unsigned char unknown000[12];
    int count;
    unsigned char unknown010[140];
    MPWeaponSlotView slots[40];
};

struct MPWeaponSetView {
    unsigned char unknown00[8];
    MPWeaponSetDataView* data;
};

struct TriggerObject_struct;

struct TriggerCoreView {
    unsigned char unknown00[72];
    int crc;
    unsigned char unknown4c[4];
    int createArgument;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

class CPlayerObject {
public:
    static int GetWeaponInfoByCRC(int, int*, int*);
};

int* CreateObject(TriggerObject_struct*, int, void*);

extern MPWeaponSetView* g_pMPWeaponSet;
extern BSObjectView* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_MPGetWeaponTypeFromWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, 0, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPGetAmmoPickupPromptFromWeaponSetID(int** stack, void*) {
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int weaponPrompt = 0;
    int ammoPrompt = 0;
    CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, &weaponPrompt, &ammoPrompt);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = ammoPrompt;
}

void BIFunc_MPGetWeaponPickupPromptFromWeaponSetID(int** stack, void*) {
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int weaponPrompt = 0;
    int ammoPrompt = 0;
    CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, &weaponPrompt, &ammoPrompt);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = weaponPrompt;
}

void BIFunc_MPCreateAmmoByType(int** stack, void*) {
    BSValueView value;
    int count = g_pMPWeaponSet->data->count;
    TriggerObject_struct* trigger = g_pBSObject->trigger;
    TriggerCoreView* core = trigger->core;
    int crc = core->crc;
    value.i = 0;
    for (int i = 0; i < count; i++) {
        if (crc == g_pMPWeaponSet->data->slots[i + 19].crc) {
            value.i = *CreateObject(trigger, core->createArgument, 0);
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateWeaponByType(int** stack, void*) {
    BSValueView value;
    int count = g_pMPWeaponSet->data->count;
    TriggerObject_struct* trigger = g_pBSObject->trigger;
    TriggerCoreView* core = trigger->core;
    int crc = core->crc;
    value.i = 0;
    for (int i = 0; i < count; i++) {
        if (crc == g_pMPWeaponSet->data->slots[i + 1].crc) {
            value.i = *CreateObject(trigger, core->createArgument, 0);
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateAmmoByWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = 0;
    if (slot > 0 && slot <= g_pMPWeaponSet->data->count) {
        g_pBSObject->trigger->core->crc = g_pMPWeaponSet->data->slots[slot + 18].crc;
        value.i = *CreateObject(g_pBSObject->trigger, g_pBSObject->trigger->core->createArgument, 0);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateWeaponByWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = 0;
    if (slot > 0 && slot <= g_pMPWeaponSet->data->count) {
        g_pBSObject->trigger->core->crc = g_pMPWeaponSet->data->slots[slot].crc;
        value.i = *CreateObject(g_pBSObject->trigger, g_pBSObject->trigger->core->createArgument, 0);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}
