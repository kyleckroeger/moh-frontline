// A fragment of renderlist.cpp (0x800f0b80): CRenderList::Finish (each
// registered bin is finished, the largest amount of the list used so far is
// kept, and true returned). The file name is this project's; the original
// record is renderlist.cpp. The classes and functions are named by the
// mangled symbols; the layouts are inferred (as in renderlist_draw.cpp). The
// bin's destructor (defined elsewhere) is declared first so no table is
// emitted here.
class CDmaPacket;

/* Inferred: a render tag's link and data. */
struct CDmaTag {
    CDmaTag* m_next;
    void* m_data;
};

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp): the
   tag chain's last and first tags at +12 and +16. */
struct CRenderBinData {
    CRenderBinData* data00;
    class CRenderBin* m_next;
    class CRenderBin* m_children;
    CDmaTag* m_lastTag;
    CDmaTag* m_firstTag;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
};

class CRenderList {
public:
    bool Finish();
    void FinishBin(CRenderBin*);

    unsigned int* m_buffer;
    void* m_end;
    unsigned char* m_current;
    CDmaTag* m_tag;
    unsigned int m_count;
    unsigned int m_maxUsed;
    CRenderBin* m_bins;
};

bool CRenderList::Finish() {
    for (CRenderBin* bin = m_bins; bin; bin = bin->m_next)
        FinishBin(bin);
    int used = m_current - (unsigned char*)m_buffer;
    if (m_maxUsed < used)
        m_maxUsed = used;
    return true;
}
