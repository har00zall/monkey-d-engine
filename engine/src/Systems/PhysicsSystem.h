#pragma once

#include "Core/System/SystemBase.h"
#include "box3d/box3d.h"

namespace MonkeyDEngine
{
    class PhysicsBody;
    class PhysicsSystem : public SystemBase
    {
    public:
        PhysicsSystem() = default;
        virtual ~PhysicsSystem() = default;

        b3WorldId GetPhysicsWorld() { return m_worldId; }

        void AddPhysicsBody(PhysicsBody *physicsBody) { m_physicsBodies.push_back(physicsBody); }

        void PhysicsUpdate();

    protected:
        b3WorldDef m_worldDefinition;
        b3WorldId m_worldId;

        std::vector<PhysicsBody *> m_physicsBodies;

        float m_physicsTimeSteps = 1.0f / 60.0f;
        float m_physicsTicks = 0;

        void OnStartSystem() override;
        void OnStopSystem() override;
        void OnPhysicsUpdate();
    };
};
