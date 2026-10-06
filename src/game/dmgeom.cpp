// Fragment of dmgeom.cpp: DMGeomSetNodeState only. DMGeomResetState, which
// follows it, is drafted in scratch but not matched yet.
// Per-node geometry state: a byte per node and a mask of changed nodes.
// Field names are descriptive.
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
