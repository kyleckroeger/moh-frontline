// Surface-type queries over the collision database's surface table. Only the
// table pointer at CDB +0x34 and the 12-byte entry fields used here are
// established; entry field names are descriptive.
struct SurfaceTypeEntryView {
    unsigned char unknownFlags : 7;
    unsigned char shootThru : 1;
    unsigned char unknown01;
    short decalIndex;
    short soundIndex;
    unsigned char unknown06[2];
    int particleCRC;
};

class CDB {
public:
    unsigned char unknown00[52];
    SurfaceTypeEntryView* surfaceTypes;
};

class CSurfaceType {
public:
    static bool IsShootThru(int);
    static int GetSoundIndex(int);
    static int GetParticleCRC(int);
    static int GetDecalIndex(int);
    static void InitClass(CDB*);
};

static CDB* g_pCDB;

bool CSurfaceType::IsShootThru(int type) {
    return g_pCDB->surfaceTypes[type].shootThru;
}

int CSurfaceType::GetSoundIndex(int type) {
    return g_pCDB->surfaceTypes[type].soundIndex;
}

int CSurfaceType::GetParticleCRC(int type) {
    return g_pCDB->surfaceTypes[type].particleCRC;
}

int CSurfaceType::GetDecalIndex(int type) {
    return g_pCDB->surfaceTypes[type].decalIndex;
}

void CSurfaceType::InitClass(CDB* cdb) {
    g_pCDB = cdb;
}
