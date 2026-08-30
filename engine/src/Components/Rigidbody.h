#pragma once

#include "PhysicsBody.h"

namespace MonkeyDEngine
{
    class Rigidbody : public PhysicsBody
    {
    protected:
        b3ShapeId collisionShapeId;

        void OnStart() override;
        void OnUpdate() override;
        void OnDestroy() override;

    public:
        Rigidbody() = default;
        ~Rigidbody() = default;
    };
};
