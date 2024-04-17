#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
    class vkCamera : public vkObject 
    {
    public:
        vkCamera();
        vkCamera(const std::string& name);
        vkCamera(const std::string& name, float fov, float aspect, float nearPlane, float farPlane);

        void SetPosition(const glm::vec3& position);
        void SetOrientation(float yaw, float pitch);
        void SetAspectRatio(float aspect);
        void UpdateCameraDelta(const glm::vec3& deltaPos, float deltaYaw, float deltaPitch);
        void UpdateCamera(const glm::vec3& pos, float yaw, float pitch);

        void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
        float GetMouseSensitivity() const { return m_MouseSensitivity; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    private:
        void UpdateView();
        void UpdateProjection();
        void UpdateCameraVectors();

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;

        float m_MouseSensitivity;

        float m_Fov;
        float m_Aspect;
        float m_NearPlane;
        float m_FarPlane;

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;

    };

}