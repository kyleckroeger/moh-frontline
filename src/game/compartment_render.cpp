// A fragment of compartment.cpp (0x800741b4): CCPTMatBin::Render (each
// queued node is added to the packet, preceded once by the pending render
// state, and unlinked; then a zero word ends the list; returns 0). The file
// name is this project's; the original record is compartment.cpp. CCPTMatBin
// and g_CurrState are named by the symbols; the node list at +104, the node
// link and the packet's templated Add (a const reference: the literal 0 is
// the image's anonymous .sdata object, linked to its pool entry) are
// inferred. g_CurrState is file-local in the original and declared extern to
// link to it. The bin's destructor (defined elsewhere) is declared first so
// no table is emitted here.
/* Inferred: the packet's write cursor at +8 and its word append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Inferred: a queued node and its link at +4. */
struct SPTNode {
    unsigned char unknown00[4];
    SPTNode* m_next;
};

extern int g_CurrState;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CCPTMatBin : public CRenderBin {
public:
    virtual ~CCPTMatBin();
    virtual int Render(CDmaPacket&, void*);

    unsigned char unknown20[72];
    SPTNode* m_nodes;
};

int CCPTMatBin::Render(CDmaPacket& packet, void*) {
    while (m_nodes) {
        if (g_CurrState) {
            packet.Add(g_CurrState);
            g_CurrState = 0;
        }
        packet.Add(m_nodes);
        SPTNode* node = m_nodes;
        m_nodes = node->m_next;
        node->m_next = 0;
    }
    packet.Add(0);
    return 0;
}
