// Character skeletons: joint lookup by name. CharSkel_t is named by the
// mangled symbols; the record members are inferred from offsets and are not
// original.
extern "C" int strcmp(const char*, const char*);

struct CharJoint_t {
    int offset;
    char name[16];
};

struct CharSkel_t {
    unsigned char field00[4];
    unsigned short flags;
    unsigned short jointcount;
    void* hierarchy;
    CharJoint_t* joints;
};

int CharSkelGetJointByName(CharSkel_t* skel, char* name) {
    CharJoint_t* joint = skel->joints;
    int i;

    for (i = 0; i < skel->jointcount; joint++, i++) {
        if (strcmp(name, joint->name) == 0)
            break;
    }
    if (i == skel->jointcount)
        i = -1;
    return i;
}

// CharRelocateSkeleton, EndianSwap(CharSkel_t&), CharTraverseSkeleton and
// _SkelRelocateHierarchyNode follow in the original file and are not
// reconstructed.
