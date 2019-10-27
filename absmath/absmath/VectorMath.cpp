#include "stdafx.h"
#include "VectorMath.h"
#include "MathDefs.h"
#include "MathUtils.h"
#include "Sphere.h"
#include "Ray.h"
#include "Plane.h"

namespace Math {

bool IsPointInTriangle(const Vector3& point, const Vector3& pa, const Vector3& pb, const Vector3& pc)
{
#if 0
    const Vector3 e10 = pb - pa;
    const Vector3 e20 = pc - pa;

    float a = e10.DotProduct(e10);
    float b = e10.DotProduct(e20);
    float c = e20.DotProduct(e20);
    float ac_bb = (a*c) - (b*b);

    const Vector3 vp(point.x_ - pa.x_, point.y_ - pa.y_, point.z_ - pa.z_);

    float d = vp.DotProduct(e10);
    float e = vp.DotProduct(e20);
    float x = (d*c) - (e*b);
    float y = (e*a) - (d*b);
    float z = x + y - ac_bb;

#define in(a) ((uint32_t&) a)
    return ((in(z) & ~(in(x) | in(y)))) & 0x80000000;
#undef in
#else
    float totalAngles = 0.0f;
    const Vector3 v1 = (point - pa).Normal();
    const Vector3 v2 = (point - pb).Normal();
    const Vector3 v3 = (point - pc).Normal();

    totalAngles += acosf(v1.DotProduct(v2));
    totalAngles += acosf(v2.DotProduct(v3));
    totalAngles += acosf(v3.DotProduct(v2));
    if (fabsf(totalAngles - M_TWOPI) <= 0.005f)
        return true;
    return false;
#endif
}

Vector3 GetClosestPointOnLine(const Vector3& a, const Vector3& b, const Vector3& p)
{
    const Vector3 c = p - a;
    Vector3 V = b - a;
    float d = V.Length();

    V.Normalize();
    float t = V.DotProduct(c);
    if (t < 0.0f)
        return a;
    if (t > d)
        return b;

    V *= t;

    return a + V;
}

Vector3 GetClosestPointOnTriangle(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p)
{
    Vector3 Rab = GetClosestPointOnLine(a, b, p);
    Vector3 Rbc = GetClosestPointOnLine(b, c, p);
    Vector3 Rca = GetClosestPointOnLine(c, a, p);

    float dAB = (p - Rab).Length();
    float dBC = (p - Rbc).Length();
    float dCA = (p - Rca).Length();

    Vector3 result = Rab;

    float m = dAB;
    if (dBC < m)
    {
        m = dBC;
        result = Rbc;
    }
    if (dCA < m)
        result = Rca;

    return result;
}

PointClass GetPointClass(const Vector3& point, const Vector3& origin, const Vector3& normal)
{
    const Vector3 dir = origin - point;
    float d = dir.DotProduct(normal);
    if (d < -0.001f)
        return PointClass::PlaneFront;
    if (d > 0.001f)
        return PointClass::PlaneBack;

    return PointClass::OnPlane;
}

}