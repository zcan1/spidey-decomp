#include "algebra.h"

#include <algorithm>

alg_real Algebra_Clamp01(alg_real value)
{
        if (value < 0)
                return 0;
#ifdef GBA_BUILD
        if (value > ALGEBRA_ONE)
                return ALGEBRA_ONE;
#else
        if (value > 1.0f)
                return 1.0f;
#endif
        return value;
}

alg_real Algebra_Lerp(alg_real from, alg_real to, alg_real t)
{
        alg_real clamped = Algebra_Clamp01(t);
        return from + Algebra_Mul((to - from), clamped);
}

alg_real Algebra_Dot(const CVector& lhs, const CVector& rhs)
{
        alg_real accum = Algebra_FixedFromFloat(0.0f);
        accum += Algebra_Mul(Algebra_FixedFromFloat(static_cast<float>(lhs.vx)),
                             Algebra_FixedFromFloat(static_cast<float>(rhs.vx)));
        accum += Algebra_Mul(Algebra_FixedFromFloat(static_cast<float>(lhs.vy)),
                             Algebra_FixedFromFloat(static_cast<float>(rhs.vy)));
        accum += Algebra_Mul(Algebra_FixedFromFloat(static_cast<float>(lhs.vz)),
                             Algebra_FixedFromFloat(static_cast<float>(rhs.vz)));
        return accum;
}
