#include "vkCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace vk
{
    vkCamera::vkCamera() : vkObject("noName", ObjectType::OT_Camera), m_Fov(60.0f), m_Aspect(16.0f / 9.0f), m_NearPlane(0.3f), m_FarPlane(10000.0f),
        m_Position(glm::vec3(0.0f, 100.0f, 0.0f)),
        m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        m_Yaw(0.0f), m_Pitch(-60.0f), m_MouseSensitivity(0.1f)
    {
        UpdateCameraVectors();
        UpdateProjection();
    }

    vkCamera::vkCamera(const std::string& name) : vkObject(name, ObjectType::OT_Camera), m_Fov(60.0f), m_Aspect(16.0f / 9.0f), m_NearPlane(0.3f), m_FarPlane(10000.0f),
        m_Position(glm::vec3(0.0f, 100.0f, 0.0f)),
        m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        m_Yaw(0.0f), m_Pitch(-60.0f), m_MouseSensitivity(0.1f)
    {
        UpdateCameraVectors();
        UpdateProjection();
    }

    vkCamera::vkCamera(const std::string& name, float fov, float aspect, float nearPlane, float farPlane)
        : vkObject(name, ObjectType::OT_Camera), m_Fov(fov), m_Aspect(aspect), m_NearPlane(nearPlane), m_FarPlane(farPlane),
        m_Position(glm::vec3(0.0f, 100.0f, 0.0f)),
        m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        m_Yaw(0.0f), m_Pitch(-60.0f), m_MouseSensitivity(0.1f)
    {
        UpdateCameraVectors();
        UpdateProjection();
    }

    void vkCamera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        UpdateView();
    }

    void vkCamera::SetOrientation(float yaw, float pitch) {
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateCameraVectors();
    }

    void vkCamera::SetAspectRatio(float aspect) {
        m_Aspect = aspect;
        UpdateProjection();
    }

    void vkCamera::UpdateCameraDelta(const glm::vec3& deltaPos, float deltaYaw, float deltaPitch)
    {
        m_Position += deltaPos;
        m_Yaw += deltaYaw;
        m_Pitch += deltaPitch;
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
        UpdateCameraVectors();
    }

    void vkCamera::UpdateCamera(const glm::vec3& pos, float yaw, float pitch)
    {
        m_Position = pos;
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateCameraVectors();
    }

    void vkCamera::UpdateView() 
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void vkCamera::UpdateProjection() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Aspect, m_NearPlane, m_FarPlane);
        // Flip Y axis for Vulkan's coordinate system
        m_ProjectionMatrix[1][1] *= -1;
    }

    void vkCamera::UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));

        UpdateView();
    }
}