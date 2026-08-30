#include "Core/System/SystemManager.h"
#include "Systems/SceneSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Transform.h"
#include "PhysicsBody.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "box3d/box3d.h"

using namespace MonkeyDEngine;

void PhysicsBody::Start()
{
    m_bodyDefinition = b3DefaultBodyDef();
    auto ownerTransform = owner->GetTransform();
    m_bodyDefinition.position = {ownerTransform->GetPosition().x, ownerTransform->GetPosition().y, ownerTransform->GetPosition().z};
    // glm::quat quaternionData = glm::quat({0, ownerTransform->GetRotation().x, ownerTransform->GetRotation().y, ownerTransform->GetRotation().z});
    // m_bodyDefinition.rotation = {.v = {.x = quaternionData.x, .y = quaternionData.y, .z = quaternionData.z}, .s = quaternionData.w};

    OnStart();

    SystemManager::Instance().GetSystem<PhysicsSystem>()->AddPhysicsBody(this);
}

void PhysicsBody::Update()
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

void PhysicsBody::OnDestroy()
{
    b3DestroyBody(m_bodyId);
}

void PhysicsBody::PhysicsUpdate()
{
    OnUpdate();
}
