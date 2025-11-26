#include "physics.h"

#include <algorithm>

static CVector gGravityVector(0, -98, 0);

static CVector toFixedVector(const CVector& source)
{
        return CVector(
                static_cast<i32>(Algebra_FixedFromFloat(static_cast<float>(source.vx))),
                static_cast<i32>(Algebra_FixedFromFloat(static_cast<float>(source.vy))),
                static_cast<i32>(Algebra_FixedFromFloat(static_cast<float>(source.vz))));
}

EXPORT void Physics_SetGravity(CVector* gravity)
{
        if (gravity)
                gGravityVector = toFixedVector(*gravity);
}

EXPORT CVector Physics_ApplyGravity(const CVector& velocity, alg_real deltaTime)
{
        CVector stepped = velocity;
        stepped.vx += Algebra_Mul(gGravityVector.vx, deltaTime);
        stepped.vy += Algebra_Mul(gGravityVector.vy, deltaTime);
        stepped.vz += Algebra_Mul(gGravityVector.vz, deltaTime);
        return stepped;
}

EXPORT PhysicsState Physics_Integrate(const PhysicsState& state, alg_real deltaTime)
{
        PhysicsState next = state;
        next.velocity = Physics_ApplyGravity(state.velocity, deltaTime);
        next.position.vx += Algebra_Mul(next.velocity.vx, deltaTime);
        next.position.vy += Algebra_Mul(next.velocity.vy, deltaTime);
        next.position.vz += Algebra_Mul(next.velocity.vz, deltaTime);
        return next;
}
