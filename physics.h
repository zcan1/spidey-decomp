#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "algebra.h"
#include "vector.h"
#include "export.h"

struct PhysicsState
{
        CVector position;
        CVector velocity;
};

EXPORT void Physics_SetGravity(CVector* gravity);
EXPORT PhysicsState Physics_Integrate(const PhysicsState& state, alg_real deltaTime);
EXPORT CVector Physics_ApplyGravity(const CVector& velocity, alg_real deltaTime);

#endif
