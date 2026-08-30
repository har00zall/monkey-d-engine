#include "Core/System/SystemManager.h"
#include "Systems/SceneSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Transform.h"
#include "Rigidbody.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "box3d/box3d.h"

using namespace MonkeyDEngine;

void Rigidbody::OnStart()
{
    PhysicsBody::OnStart();

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

    // b3Sphere sphere;
    // sphere.center = {0.0f, 3.0f, 0.0f};
    // sphere.radius = 3.0f;
    // b3ShapeDef shapeDef = b3DefaultShapeDef();
    // shapeDef.density = 2.0f;
    // collisionShapeId = b3CreateSphereShape(m_bodyId, &shapeDef, &sphere);

    b3MassData massData = {};
    massData.center = {.x = 0, .y = 0, .z = 0};
    massData.inertia = b3Mat3_identity;
    massData.mass = 10.0f;

    b3Body_SetMassData(m_bodyId, massData);
    b3Body_ApplyMassFromShapes(m_bodyId);
}

void Rigidbody::OnUpdate()
{
    auto position = b3Body_GetPosition(m_bodyId);
    auto rotation = b3Body_GetRotation(m_bodyId);
    glm::quat quaternionData = glm::quat({rotation.s, rotation.v.x, rotation.v.y, rotation.v.z});
    glm::vec3 eulerData = glm::eulerAngles(quaternionData);
    eulerData = glm::degrees(eulerData);
    // SDL_Log("RotationData: %f %f %f", eulerData.x, eulerData.y, eulerData.z);
    owner->GetTransform()
        ->SetPosition({position.x, position.y, position.z});
    owner->GetTransform()->SetRotation(eulerData);
}

void Rigidbody::OnDestroy()
{
    PhysicsBody::OnDestroy();
}
