// A fragment of bsmachin.cpp (0x800383f0): the behaviour-script machine's
// memory requirement and the property-data readers (LoadPropertyData and the
// per-structure _LoadPropertyData overloads), which return a field of a
// level object's property structure by index. The file name is this
// project's; the original record is bsmachin.cpp, and BSInitMachine before
// these is not reconstructed (the rest of the file is the bsmachin unit). The
// functions and structures are named by the mangled symbols; the structure
// members are inferred from the accessed offsets, and the constants are
// entries of the file's .sdata2 pool. Structure and descriptive names follow
// the Rising Sun reconstruction (EA's later revision).
// Behaviour-script virtual machine. Structure and descriptive names follow the
// Rising Sun reconstruction (EA's later revision); only accessed fields and
// strides are established.


struct xyzProperty_Struct {
    unsigned char unknown00[12];
    int field0c;
};

// Property-data readers. Field names give the byte offset; only the
// accessed members of each structure are established.
struct MOH_fogParams_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    short field70;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};
struct MOH_animatedLight_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char unknown71[1];
    short field72;
};
struct MOH_playerPathController_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};
struct MOH_cameraShaker_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
};
struct MOH_projectileGeneratorTarget_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
    int field88;
};
struct MOH_projectileGenerator_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
    int field88;
    int field8c;
    int field90;
    int field94;
};
struct MOH_particle_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char field72;
    unsigned char field73;
    unsigned char field74;
    unsigned char field75;
    unsigned char field76;
    unsigned char field77;
    unsigned char field78;
    unsigned char field79;
    unsigned char field7a;
    unsigned char field7b;
    unsigned char field7c;
    unsigned char field7d;
    unsigned char field7e;
    unsigned char field7f;
    unsigned char field80;
    unsigned char unknown81[1];
    short field82;
    short field84;
    unsigned char unknown86[2];
    int field88;
    int field8c;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
    int fieldb0;
    int fieldb4;
    int fieldb8;
    int fieldbc;
    int fieldc0;
    int fieldc4;
    int fieldc8;
    int fieldcc;
    int fieldd0;
    int fieldd4;
    int fieldd8;
    int fielddc;
    int fielde0;
    int fielde4;
    int fielde8;
};
struct MOH_mechanicEnvMod_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
    int field78;
};
struct MOH_envMod_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    int field70;
};
struct MOH_pathPoint_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
};
struct MOH_wayPoint_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
};
struct MOH_powerup_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
};
struct MOH_enemy_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char field72;
    unsigned char field73;
    unsigned char field74;
    unsigned char field75;
    unsigned char field76;
    unsigned char field77;
    unsigned char field78;
    unsigned char field79;
    unsigned char field7a;
    unsigned char field7b;
    unsigned char field7c;
    unsigned char unknown7d[1];
    short field7e;
    short field80;
    short field82;
    short field84;
    short field86;
    short field88;
    short field8a;
    short field8c;
    short field8e;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
};
struct MOH_mechanic_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
};
struct MOH_core_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
};


int _LoadPropertyData(MOH_fogParams_Struct* object, int field);
int _LoadPropertyData(MOH_animatedLight_Struct* object, int field);
int _LoadPropertyData(MOH_playerPathController_Struct* object, int field);
int _LoadPropertyData(MOH_cameraShaker_Struct* object, int field);
int _LoadPropertyData(MOH_projectileGeneratorTarget_Struct* object, int field);
int _LoadPropertyData(MOH_projectileGenerator_Struct* object, int field);
int _LoadPropertyData(MOH_particle_Struct* object, int field);
int _LoadPropertyData(MOH_mechanicEnvMod_Struct* object, int field);
int _LoadPropertyData(MOH_envMod_Struct* object, int field);
int _LoadPropertyData(MOH_pathPoint_Struct* object, int field);
int _LoadPropertyData(MOH_wayPoint_Struct* object, int field);
int _LoadPropertyData(MOH_powerup_Struct* object, int field);
int _LoadPropertyData(MOH_enemy_Struct* object, int field);
int _LoadPropertyData(MOH_mechanic_Struct* object, int field);
int _LoadPropertyData(MOH_core_Struct* object, int field);

int LoadPropertyData(xyzProperty_Struct* properties, int field);

// Only the accessed members are established.
struct TriggerObject_struct {
    unsigned char unknown00[4];
    xyzProperty_Struct* properties;
};

struct BSScriptClassView {
    unsigned char unknown00[96];
    int* sharedValues;
};

class BSObjectUserView;

struct BSObject {
    BSScriptClassView* scriptClass;
    unsigned char unknown04[4];
    TriggerObject_struct* nativeObject;
    BSObjectUserView* user;
    int queueIdentity;
};

struct BSClass_struct;

// Only the accessed members and strides are established.
struct BSClassListView {
    unsigned char unknown00[4];
    BSClass_struct* next;
};

struct BSCode_MessageHandlerEntry_struct {
    unsigned char unknown00[12];
};

struct BSMessageRegistration_struct;

struct BSEventView {
    unsigned int code;
    unsigned short eventNumber;
    unsigned char unknown06;
    unsigned char flags;
};

struct BSStateView {
    unsigned int code;
    BSEventView* events;
    BSCode_MessageHandlerEntry_struct* messages;
    unsigned short parent;
    unsigned char depth;
    unsigned char messageCount;
    unsigned char eventCount;
};

struct BSMessageRegistrationView {
    unsigned char unknown00[22];
    unsigned short flags;
};

struct BSCode_struct {
    int** codeBases;
    unsigned char unknown04[4];
    BSStateView** states;
    unsigned char unknown0c[10];
    unsigned short initialState;
};

struct BSMessageListView {
    BSMessageRegistration_struct* handler;
    BSMessageRegistration_struct* registration;
    BSMessageListView* next;
    unsigned short depth;
    unsigned short stateId;
};

struct BSMachineThread_struct {
    int* frame;
    int* stackTop;
    void* context;
    int* instruction;
    BSStateView* state;
    BSCode_struct* level;
    BSMessageListView* messages;
    unsigned short stateId;
    unsigned short flags;
};

// The object at BSObject+12; only the third virtual slot is called here, and
// its name is unknown.
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
    virtual void unknownVirtual1();
    virtual void* unknownVirtual2();
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[12];
};

extern int g_iBSMessageRegistrationListSize;
extern BSClass_struct* g_pcClassList;
extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;


extern int* g_piStackTop;
extern int* g_piStackBase;
extern BSObject* g_pBSObject;
extern int* g_piIP;
extern int* g_piFrame;
extern int g_iStopThread;
extern int** g_piCodeBase;
extern int* g_piGlobals;
extern BSStateView** g_ppsteStateEntries;
extern BSMachineThread_struct* g_pbmtThread;
extern void* g_pObj;
extern BSMessageListView* g_MessageList;
extern BSMessageListView* g_MessageListHead;
extern unsigned char* g_pBSMachineMemory;
extern int g_pBSMachineMemoryOffset;
extern int g_iDefaultStackSize;

void BSMessageRemoveHandler(BSMessageRegistration_struct*);
BSMessageRegistration_struct* BSRegisterMessage(BSCode_MessageHandlerEntry_struct*, BSObject*, unsigned short,
                                                BSMachineThread_struct*, BSMessageRegistration_struct**);
void BSFileDeleteClass(BSClass_struct*);
void* BSUtilGetMemory(int, int);

static BSMessageListView* BSMachineGetFreeMessageList();

// The signed argument occupies the high half of the instruction word.
inline int BSInstructionArgument() {
    return *g_piIP >> 16;
}

void BSOpCodeFunc_NEXTSTATE();

void BSOpCodeFunc_TRACE();

void BSOpCodeFunc_BREAK();

void BSOpCodeFunc_RETURN();

void BSOpCodeFunc_BIFNCALL();

void BSOpCodeFunc_FNCALL();

void BSOpCodeFunc_GP();

void BSOpCodeFunc_LS();

void BSOpCodeFunc_LM();

void BSOpCodeFunc_SM();

void BSOpCodeFunc_LG();

void BSOpCodeFunc_SG();

void BSOpCodeFunc_LW();

void BSOpCodeFunc_SW();

void BSOpCodeFunc_PUSH();

void BSOpCodeFunc_POP();

void BSOpCodeFunc_CAST();

void BSOpCodeFunc_NE();

void BSOpCodeFunc_EQ();

void BSOpCodeFunc_LTE();

void BSOpCodeFunc_LT();

void BSOpCodeFunc_GTE();

void BSOpCodeFunc_GT();

void BSOpCodeFunc_NOT();

void BSOpCodeFunc_OR();

void BSOpCodeFunc_AND();

void BSOpCodeFunc_NEG();

void BSOpCodeFunc_MULT();

void BSOpCodeFunc_DIV();

void BSOpCodeFunc_SUB();

void BSOpCodeFunc_ADD();

void BSOpCodeFunc_JMP();

void BSOpCodeFunc_BNE();

int BSExecuteThread(BSObject* object, BSMachineThread_struct* thread);

int BSDestroyThread(BSMachineThread_struct* thread);

int BSCreateThread(BSObject* object, BSCode_struct* level, BSMachineThread_struct* thread, void* context);

int BSEndMachine();

int BSInitMachine();

int BSMachineGetMemoryRequirements() {
    return g_iDefaultStackSize * sizeof(int) + g_iBSMessageRegistrationListSize * sizeof(BSMessageListView);
}

int LoadPropertyData(xyzProperty_Struct* properties, int field) {
    switch (properties->field0c) {
    case 1:
        return _LoadPropertyData(reinterpret_cast<MOH_core_Struct*>(properties), field);
    case 2:
        return _LoadPropertyData(reinterpret_cast<MOH_mechanic_Struct*>(properties), field);
    case 3:
        return _LoadPropertyData(reinterpret_cast<MOH_enemy_Struct*>(properties), field);
    case 4:
        return _LoadPropertyData(reinterpret_cast<MOH_powerup_Struct*>(properties), field);
    case 5:
        return _LoadPropertyData(reinterpret_cast<MOH_wayPoint_Struct*>(properties), field);
    case 6:
        return _LoadPropertyData(reinterpret_cast<MOH_pathPoint_Struct*>(properties), field);
    case 7:
        return _LoadPropertyData(reinterpret_cast<MOH_envMod_Struct*>(properties), field);
    case 8:
        return _LoadPropertyData(reinterpret_cast<MOH_mechanicEnvMod_Struct*>(properties), field);
    case 9:
        return _LoadPropertyData(reinterpret_cast<MOH_particle_Struct*>(properties), field);
    case 10:
        return _LoadPropertyData(reinterpret_cast<MOH_projectileGenerator_Struct*>(properties), field);
    case 11:
        return _LoadPropertyData(reinterpret_cast<MOH_projectileGeneratorTarget_Struct*>(properties), field);
    case 12:
        return _LoadPropertyData(reinterpret_cast<MOH_cameraShaker_Struct*>(properties), field);
    case 13:
        return _LoadPropertyData(reinterpret_cast<MOH_playerPathController_Struct*>(properties), field);
    case 14:
        return _LoadPropertyData(reinterpret_cast<MOH_animatedLight_Struct*>(properties), field);
    case 15:
        return _LoadPropertyData(reinterpret_cast<MOH_fogParams_Struct*>(properties), field);
    }
    return 0;
}

int _LoadPropertyData(MOH_fogParams_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field78;
    case 23:
        return object->field7c;
    case 24:
        return object->field70;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    }
    return 0;
}

int _LoadPropertyData(MOH_animatedLight_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 24:
        return object->field70;
    case 25:
        return object->field72;
    }
    return 0;
}

int _LoadPropertyData(MOH_playerPathController_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field7c;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    }
    return 0;
}

int _LoadPropertyData(MOH_cameraShaker_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field84;
    case 24:
        return object->field71;
    case 25:
        return object->field74;
    case 26:
        return object->field78;
    case 27:
        return object->field7c;
    case 28:
        return object->field80;
    }
    return 0;
}

int _LoadPropertyData(MOH_projectileGeneratorTarget_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field88;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    case 28:
        return object->field7c;
    case 29:
        return object->field80;
    case 30:
        return object->field84;
    }
    return 0;
}

int _LoadPropertyData(MOH_projectileGenerator_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field90;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    case 28:
        return object->field7c;
    case 29:
        return object->field80;
    case 30:
        return object->field94;
    case 31:
        return object->field84;
    case 32:
        return object->field88;
    case 33:
        return object->field8c;
    }
    return 0;
}

int _LoadPropertyData(MOH_particle_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 23:
        return object->field82;
    case 24:
        return object->field84;
    case 25:
        return object->field88;
    case 26:
        return object->field8c;
    case 27:
        return object->field90;
    case 28:
        return object->field94;
    case 29:
        return object->field98;
    case 30:
        return object->field9c;
    case 31:
        return object->fielda0;
    case 32:
        return object->fielda4;
    case 33:
        return object->fielda8;
    case 34:
        return object->fieldac;
    case 35:
        return object->fieldb0;
    case 36:
        return object->fieldb4;
    case 37:
        return object->fieldb8;
    case 38:
        return object->fieldbc;
    case 39:
        return object->fieldc0;
    case 40:
        return object->fieldc4;
    case 41:
        return object->fieldc8;
    case 42:
        return object->field71;
    case 43:
        return object->field72;
    case 44:
        return object->field73;
    case 45:
        return object->field74;
    case 46:
        return object->field75;
    case 47:
        return object->field76;
    case 48:
        return object->field77;
    case 49:
        return object->field78;
    case 50:
        return object->field79;
    case 51:
        return object->fieldcc;
    case 52:
        return object->fieldd0;
    case 53:
        return object->fieldd4;
    case 54:
        return object->fieldd8;
    case 55:
        return object->fielddc;
    case 56:
        return object->fielde0;
    case 57:
        return object->fielde4;
    case 58:
        return object->fielde8;
    case 59:
        return object->field7a;
    case 60:
        return object->field7b;
    case 61:
        return object->field7c;
    case 62:
        return object->field7d;
    case 63:
        return object->field7e;
    case 64:
        return object->field7f;
    case 65:
        return object->field80;
    case 66:
        return object->field70 & 1;
    }
    return 0;
}

int _LoadPropertyData(MOH_mechanicEnvMod_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field74;
    case 24:
        return object->field71;
    case 25:
        return object->field78;
    }
    return 0;
}

int _LoadPropertyData(MOH_envMod_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    }
    return 0;
}

int _LoadPropertyData(MOH_pathPoint_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    }
    return 0;
}

int _LoadPropertyData(MOH_wayPoint_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70 & 1;
    case 23:
        return (object->field70 >> 1) & 1;
    case 24:
        return (object->field70 >> 2) & 1;
    case 25:
        return (object->field70 >> 3) & 1;
    case 26:
        return (object->field70 >> 4) & 1;
    case 27:
        return (object->field70 >> 5) & 1;
    case 28:
        return (object->field70 >> 6) & 1;
    case 29:
        return (object->field70 >> 7) & 1;
    }
    return 0;
}

int _LoadPropertyData(MOH_powerup_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    }
    return 0;
}

int _LoadPropertyData(MOH_enemy_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field90;
    case 24:
        return object->field71;
    case 25:
        return object->field94;
    case 26:
        return object->field72;
    case 27:
        return object->field98;
    case 28:
        return object->field9c;
    case 29:
        return object->fielda0;
    case 30:
        return object->field73;
    case 31:
        return object->field74;
    case 32:
        return object->field75;
    case 33:
        return object->fielda4;
    case 34:
        return object->fielda8;
    case 35: {
        float value = object->field7e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 36: {
        float value = object->field80 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 37: {
        float value = object->field82 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 38:
        return object->field76;
    case 39:
        return object->field77;
    case 40:
        return object->field78;
    case 41:
        return object->field79;
    case 42:
        return object->field84;
    case 43:
        return object->field7a;
    case 44:
        return object->fieldac;
    case 45: {
        float value = object->field86 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 46: {
        float value = object->field88 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 47: {
        float value = object->field8a / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 48: {
        float value = object->field8c / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 49: {
        float value = object->field8e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 50:
        return object->field7b;
    case 51:
        return object->field7c;
    }
    return 0;
}

int _LoadPropertyData(MOH_mechanic_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field74;
    case 24:
        return object->field71;
    }
    return 0;
}

int _LoadPropertyData(MOH_core_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    }
    return 0;
}



