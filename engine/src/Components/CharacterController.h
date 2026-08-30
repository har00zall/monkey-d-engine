#pragma once

#include "Core/TransformTypeDef.h"
#include "PhysicsBody.h"

namespace MonkeyDEngine
{
    class CharacterController : public PhysicsBody
    {
    protected:
        void OnUpdate() override;
        void OnStart() override;

    public:
        CharacterController() = default;
        ~CharacterController() = default;

        void Move(Vector3 motion);
    };
};
