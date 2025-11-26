#pragma once

#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <cstdint>

#include "vector.h"

// Shared precision controls for platforms that rely on fixed-point math. GBA
// builds enable GBA_BUILD to force fixed-point conversions while desktop builds
// continue using floats with identical call sites.
#ifdef GBA_BUILD
using alg_real = int32_t;
constexpr int ALGEBRA_SHIFT = 12;
constexpr alg_real ALGEBRA_ONE = 1 << ALGEBRA_SHIFT;

INLINE alg_real Algebra_FixedFromFloat(float value)
{
        return static_cast<alg_real>(value * static_cast<float>(ALGEBRA_ONE));
}

INLINE float Algebra_FloatFromFixed(alg_real value)
{
        return static_cast<float>(value) / static_cast<float>(ALGEBRA_ONE);
}

INLINE alg_real Algebra_Mul(alg_real lhs, alg_real rhs)
{
        return static_cast<alg_real>((static_cast<int64_t>(lhs) * rhs) >> ALGEBRA_SHIFT);
}

INLINE alg_real Algebra_Div(alg_real lhs, alg_real rhs)
{
        return static_cast<alg_real>((static_cast<int64_t>(lhs) << ALGEBRA_SHIFT) / rhs);
}
#else
using alg_real = float;
constexpr int ALGEBRA_SHIFT = 0;
constexpr alg_real ALGEBRA_ONE = 1.0f;

INLINE alg_real Algebra_FixedFromFloat(float value)
{
        return value;
}

INLINE float Algebra_FloatFromFixed(alg_real value)
{
        return value;
}

INLINE alg_real Algebra_Mul(alg_real lhs, alg_real rhs)
{
        return lhs * rhs;
}

INLINE alg_real Algebra_Div(alg_real lhs, alg_real rhs)
{
        return lhs / rhs;
}
#endif

alg_real Algebra_Lerp(alg_real from, alg_real to, alg_real t);
alg_real Algebra_Clamp01(alg_real value);
alg_real Algebra_Dot(const CVector& lhs, const CVector& rhs);

#endif
