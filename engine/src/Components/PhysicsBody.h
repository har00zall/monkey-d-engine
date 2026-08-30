#pragma once

#include "Component.h"
#include "box3d/box3d.h"

namespace MonkeyDEngine
{
    class PhysicsBody : public Component
    {
    protected:
        b3BodyDef m_bodyDefinition = {};
        b3BodyId m_bodyId = {};
        b3ShapeId collisionShapeId = {};

        void virtual OnStart() override;
        void virtual OnPhysicsUpdate();
        void OnUpdate() override;

        void UpdateWorldPosition();
        void UpdateWorldRotation();

    public:
        PhysicsBody() = default;
        ~PhysicsBody() = default;
        void OnDestroy() override;

        void PhysicsUpdate();
    };
};
