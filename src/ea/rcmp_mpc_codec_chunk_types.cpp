namespace {
const unsigned int ChunkTypes[1] = { 0x4D504368 };  // 'MPCh'
}

class RCMP {
public:
    static int MPC_CODEC_is_chunk_for_codec(unsigned int);
};

int RCMP::MPC_CODEC_is_chunk_for_codec(unsigned int type) {
    for (int i = 0; i < (int)(sizeof(ChunkTypes) / sizeof(ChunkTypes[0])); i++) {
        if (type == ChunkTypes[i])
            return 1;
    }
    return 0;
}
