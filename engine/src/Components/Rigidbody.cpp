#include "Core/System/SystemManager.h"
#include "Systems/PhysicsSystem.h"
#include "Transform.h"
#include "Rigidbody.h"

#include "box3d/box3d.h"

using namespace MonkeyDEngine;

void Rigidbody::OnStart()
{
    m_bodyDefinition.type = b3_dynamicBody;
    m_bodyDefinition.isEnabled = true;
    m_bodyDefinition.gravityScale = 1.0f;

    m_bodyId = b3CreateBody(SystemManager::Instance().GetSystem<PhysicsSystem>()->GetPhysicsWorld(), &m_bodyDefinition);

    // b3Capsule capsule;
    // capsule.center1 = {0.0f, -3.0f, 0.0f};
    // capsule.center2 = {0.0f, 6.0f, 0.0f};
    // capsule.radius = 1.0f;
    // b3ShapeDef shapeDef = b3DefaultShapeDef();
    // shapeDef.density = 2.0f;
    // collisionShapeId = b3CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);

    b3BoxHull boxHull = b3MakeBoxHull(2.0f, 5.0f, 2.0f);
    b3ShapeDef boxShapeDef = b3DefaultShapeDef();
    boxShapeDef.density = 2.0f;
    boxShapeDef.baseMaterial.friction = 0.3f;
    collisionShapeId = b3CreateHullShape(m_bodyId, &boxShapeDef, &boxHull.base);

    b3MassData massData = {};
    massData.center = {.x = 0, .y = 0, .z = 0};
    massData.inertia = b3Mat3_identity;
    massData.mass = 10.0f;

    b3Body_SetMassData(m_bodyId, massData);
    b3Body_ApplyMassFromShapes(m_bodyId);
}

void Rigidbody::OnUpdate()
{
}

void Rigidbody::OnDestroy()
{
    b3DestroyShape(collisionShapeId, true);

    PhysicsBody::OnDestroy();
}
