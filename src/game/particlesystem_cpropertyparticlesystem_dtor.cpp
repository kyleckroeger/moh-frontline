// A fragment of particlesystem.cpp (0x8007f364): the CPropertyParticleSystem destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, CParticleSystem, and the object freed when asked). The file
// name is this project's; the original record is particlesystem.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CPropertyParticleSystem declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// CRenderBin: weak table (no key function) and inline destructor, so
// the compiler's copies are weak duplicates, linked to the original copies.
// The destructor of CParticleSystem is global in the original (out of line,
// elsewhere in the same file) yet inlined here; how the original made it available
// inline is not known, so the view declares it inline, after another of
// the class's own virtual functions so that its global table is not emitted.
class CDmaPacket;

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
};

class CParticleSystem : public CRenderBin {
public:
    virtual void DeActivate(); /* result type not known */
    virtual ~CParticleSystem() {}
};

class CPropertyParticleSystem : public CParticleSystem {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPropertyParticleSystem();
};

CPropertyParticleSystem::~CPropertyParticleSystem() {
}
