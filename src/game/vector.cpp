// CVector3::Constrain and CVector2::Rotate (the class members and inline
// operators are a minimal inferred view). Constrain turns this vector toward
// target (both unit length) until the cosine of the angle between them reaches
// cosLimit, by spherical interpolation; Rotate turns a 2D vector by a 24-bit
// fixed-point angle.
extern "C" {
double acos(double);
double sin(double);
}
void MathSinCos(long, float*, float*);

class CVector3 {
public:
    float x;
    float y;
    float z;

    CVector3() {}
    CVector3& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    void Constrain(const CVector3& target, float cosLimit);
};

inline CVector3 operator*(float s, const CVector3& v) {
    CVector3 r;
    r.x = s * v.x;
    r.y = s * v.y;
    r.z = s * v.z;
    return r;
}

class CVector2 {
public:
    float x;
    float y;

    void Rotate(const CVector2& source, long angle);
};

void CVector3::Constrain(const CVector3& target, float cosLimit) {
    float cosAngle = x * target.x + y * target.y + z * target.z;

    if (cosAngle < cosLimit) {
        float limit = acos(cosLimit);
        float angle = acos(cosAngle);
        float sinAngle = sin(angle);
        float targetWeight = (float)sin(angle - limit) / sinAngle;
        float selfWeight = (float)sin(limit) / sinAngle;

        *this *= selfWeight;
        *this += targetWeight * target;
    }
}

void CVector2::Rotate(const CVector2& source, long angle) {
    float sx = source.x;
    float sy = source.y;
    float s;
    float c;

    MathSinCos(angle, &s, &c);
    x = c * sx - s * sy;
    y = c * sy + s * sx;
}
