#pragma once

#include "Component.h"
#include "box3d/box3d.h"

namespace MonkeyDEngine
{
    class PhysicsBody : public Component
    {
    protected:
        b3BodyDef m_bodyDefinition;
        b3BodyId m_bodyId;

        void virtual OnStart() = 0;
        void virtual OnUpdate() = 0;

    public:
        PhysicsBody() = default;
        ~PhysicsBody() = default;

        void Start() override;
        void Update() override;
        void OnDestroy() override;

        void PhysicsUpdate();
    };
};
