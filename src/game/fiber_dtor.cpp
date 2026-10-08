// A fragment of fiber.cpp (0x800ca504): the CVolFiber destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked) and the constructors: from a line (copied), from a
// start and an end (the line set from them: direction end - start, flags
// cleared) and the copy constructor (copies only the line). The file name is
// this project's; the original record is fiber.cpp, and these functions
// follow fiber_create.cpp. IVolume, CVolFiber, CLine3 and CVector3 are named
// by the mangled symbols; the CLine3 and CVector3 views are as in
// fiber_create.cpp, with an inferred member-wise copy constructor and an
// inferred inline constructor from two points (through SetSE), and the
// fiber's members are inferred. Only the virtuals these functions need are
// declared. CVolFiber declares Create (defined elsewhere) before its
// destructor, so its own virtual table, which belongs with the destructor in
// the original file, is not emitted twice: the override keeps IVolume's slot
// order either way. IVolume's destructor is inline (weak in the original), so
// the compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(const CLine3& other)
        : m_start(other.m_start), m_end(other.m_end), m_dir(other.m_dir), m_value30(other.m_value30),
          m_value34(other.m_value34), m_flag38(other.m_flag38), m_flag39(other.m_flag39) {}
    CLine3() {}
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.v[0] = m_end.v[0] - m_start.v[0];
        m_dir.v[1] = m_end.v[1] - m_start.v[1];
        m_dir.v[2] = m_end.v[2] - m_start.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolFiber : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolFiber();
    CVolFiber(const CLine3&);
    CVolFiber(CVector3, CVector3);
    CVolFiber(const CVolFiber&);

    unsigned char unknown04[12];
    CLine3 m_line;
};

CVolFiber::~CVolFiber() {
}

CVolFiber::CVolFiber(const CLine3& line) : m_line(line) {
}

CVolFiber::CVolFiber(CVector3 start, CVector3 end) : m_line(start, end) {
}

CVolFiber::CVolFiber(const CVolFiber& other) : m_line(other.m_line) {
}
