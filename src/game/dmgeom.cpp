// Fragment of dmgeom.cpp: DMGeomSetNodeState (a node's state byte changed and
// its bit set in the mask of changed nodes when the value differs) and
// DMGeomResetState (every node reset to 0xff and marked changed, through a
// pointer to the node byte). DMGeomBuildPartList, which follows, is drafted
// in scratch (scratch/dmgeom_buildpartlist24.cpp). The functions and
// DMGeometryState_T are named by the mangled symbols; the field names are
// descriptive.
struct DMGeometryState_T {
    unsigned long dirty;
    unsigned char nodes[32];
};

void DMGeomSetNodeState(DMGeometryState_T* state, long node, long value) {
    if (state->nodes[node] != value) {
        state->nodes[node] = value;
        state->dirty |= 1 << node;
    }
}

void DMGeomResetState(DMGeometryState_T* state) {
    for (int i = 0; i < 32; i++) {
        unsigned char* node = &state->nodes[i];
        *node = 0xff;
        state->dirty |= 1 << i;
    }
}
