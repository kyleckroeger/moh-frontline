// A fragment of staticmesh.cpp (0x800f1c58): CMSHMatBin::Render (for each
// queued mesh node: the pending render state once, the world-to-camera
// matrix in GX 3x4 form copied element by element, the lighting parameters
// and the node, which is then unlinked; a zero word ends the list; returns
// 0). The file name is this project's; the original record is staticmesh.cpp.
// CMSHMatBin, g_CurrState, g_WorldToCameraMatrix and UpdateLightingParams
// are named by the symbols; the node list at +104, the node link and the
// packet's templated Add (as in compartment_render.cpp; the literal 0 is the
// image's anonymous .sdata object, linked to its pool entry) are inferred.
// The globals are file-local in the original and declared extern to link to
// them. The bin's destructor (defined elsewhere) is declared first so no
// table is emitted here.
/* Inferred: the packet's write cursor at +8 and its templated append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

class CMatrix {
public:
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;

    float m[4][4];
} __attribute__((aligned(16)));

/* Inferred: a queued node and its link at +16. */
struct SMSHNode {
    unsigned char unknown00[16];
    SMSHNode* m_next;
};

extern int g_CurrState;
extern CMatrix g_WorldToCameraMatrix;

void UpdateLightingParams(CDmaPacket&);

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CMSHMatBin : public CRenderBin {
public:
    virtual ~CMSHMatBin();
    virtual int Render(CDmaPacket&, void*);

    unsigned char unknown20[72];
    SMSHNode* m_nodes;
};

int CMSHMatBin::Render(CDmaPacket& packet, void*) {
    while (m_nodes) {
        if (g_CurrState) {
            packet.Add(g_CurrState);
            g_CurrState = 0;
        }
        float matrix[3][4];
        g_WorldToCameraMatrix.GetGAMECUBEMatrix34(matrix);
        float (*dst)[4] = (float (*)[4])packet.m_cur;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++)
                dst[i][j] = matrix[i][j];
        packet.m_cur += sizeof(matrix);
        UpdateLightingParams(packet);
        packet.Add(m_nodes);
        SMSHNode* node = m_nodes;
        m_nodes = node->m_next;
        node->m_next = 0;
    }
    packet.Add(0);
    return 0;
}
