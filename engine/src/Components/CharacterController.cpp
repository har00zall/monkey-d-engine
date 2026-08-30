#include "Core/System/SystemManager.h"
#include "Core/TransformTypeDef.h"
#include "Systems/PhysicsSystem.h"

#include "CharacterController.h"

using namespace MonkeyDEngine;

void CharacterController::OnStart()
{
    m_bodyDefinition.type = b3_kinematicBody;
    m_bodyDefinition.isEnabled = true;
    m_bodyDefinition.gravityScale = 0.0f;

    m_bodyId = b3CreateBody(SystemManager::Instance().GetSystem<PhysicsSystem>()->GetPhysicsWorld(), &m_bodyDefinition);

    b3Capsule capsule;
    capsule.center1 = {0.0f, -3.0f, 0.0f};
    capsule.center2 = {0.0f, 6.0f, 0.0f};
    capsule.radius = 1.0f;
    b3ShapeDef shapeDef = b3DefaultShapeDef();
    shapeDef.density = 2.0f;
    collisionShapeId = b3CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
}

void CharacterController::OnUpdate()
{
    // UpdateWorldPosition();
}

void CharacterController::Move(Vector3 motion)
{
    // b3Body_SetLinearVelocity(m_bodyId, {motion.x, motion.y, motion.z});
}
