#include "Core/OS/Time.h"
#include "Components/PhysicsBody.h"

#include "PhysicsSystem.h"

using namespace MonkeyDEngine;

void PhysicsSystem::PhysicsUpdate()
{
    m_physicsTicks += Time::deltaTime;

    if (m_physicsTicks >= m_physicsTimeSteps)
    {
        m_physicsTicks -= m_physicsTimeSteps;
        // OnPhysicsUpdate();
    }

    b3World_Step(m_worldId, m_physicsTimeSteps, 4);
}

void PhysicsSystem::OnStartSystem()
{
    // creating world
    m_worldDefinition = b3DefaultWorldDef();
    m_worldDefinition.gravity = {0.0f, -9.8f, 0.0f};
    m_worldId = b3CreateWorld(&m_worldDefinition);

    // TODO: remove this and make it into separate component
    {
        b3BodyDef bodyDef = b3DefaultBodyDef();
        bodyDef.type = b3_staticBody;
        bodyDef.isEnabled = true;
        bodyDef.position = {-10.0f, -10.0f, 25.0f};
        bodyDef.name = "platform";
        auto m_platformId = b3CreateBody(m_worldId, &bodyDef);

        b3BoxHull box = b3MakeTransformedBoxHull(
            2.f, 100.0f, 100.0f, {{0.0f, 0.0f, 0.0f}, b3MakeQuatFromAxisAngle(b3Vec3_axisZ, 0.5f * B3_PI)});

        b3ShapeDef shapeDef = b3DefaultShapeDef();
        shapeDef.density = 2.0f;
        shapeDef.baseMaterial.restitution = 0.5f;
        b3CreateHullShape(m_platformId, &shapeDef, &box.base);
    }
}

void PhysicsSystem::OnStopSystem()
{
    b3DestroyWorld(m_worldId);
}

void PhysicsSystem::OnPhysicsUpdate()
{
    for (auto body : m_physicsBodies)
    {
        body->PhysicsUpdate();
    }
}
