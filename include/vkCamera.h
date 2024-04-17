#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vkObject.h"

namespace vk
{
    class vkCamera : public vkObject 
    {
    public:
        vkCamera() : vkObject("noName", ObjectType::OT_Camera), m_Fov(60.0f), m_Aspect(16.0f/9.0f), m_NearPlane(0.3f), m_FarPlane(10000.0f),
            m_Position(glm::vec3(0.0f, 0.0f, 0.0f)),
            m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
            m_Yaw(0.0f), m_Pitch(-90.0f)
        {
            UpdateCameraVectors();
            UpdateProjection();
        }

        vkCamera(const std::string& name) : vkObject(name, ObjectType::OT_Camera), m_Fov(60.0f), m_Aspect(16.0f / 9.0f), m_NearPlane(0.3f), m_FarPlane(10000.0f),
            m_Position(glm::vec3(0.0f, 0.0f, 0.0f)),
            m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
            m_Yaw(0.0f), m_Pitch(-90.0f)
        {
            UpdateCameraVectors();
            UpdateProjection();
        }

        vkCamera(const std::string& name, float fov, float aspect, float nearPlane, float farPlane)
            : vkObject(name, ObjectType::OT_Camera), m_Fov(fov), m_Aspect(aspect), m_NearPlane(nearPlane), m_FarPlane(farPlane),
            m_Position(glm::vec3(0.0f, 0.0f, 0.0f)),
            m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
            m_Yaw(0.0f), m_Pitch(-90.0f) 
        {
            UpdateCameraVectors();
            UpdateProjection();
        }

        void SetPosition(const glm::vec3& position) {
            m_Position = position;
            UpdateView();
        }

        void SetOrientation(float yaw, float pitch) {
            m_Yaw = yaw;
            m_Pitch = pitch;
            UpdateCameraVectors();
        }

        void SetAspectRatio(float aspect) {
            m_Aspect = aspect;
            UpdateProjection();
        }

        const glm::mat4& GetViewMatrix() const {
            return m_ViewMatrix;
        }

        const glm::mat4& GetProjectionMatrix() const {
            return m_ProjectionMatrix;
        }

        // Implement camera movement functions such as moveForward, moveBackward, etc.
        // ...

    private:
        void UpdateView() {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }

        void UpdateProjection() {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Aspect, m_NearPlane, m_FarPlane);
            // Flip Y axis for Vulkan's coordinate system
            m_ProjectionMatrix[1][1] *= -1;
        }

        void UpdateCameraVectors() {
            glm::vec3 front;
            front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            front.y = sin(glm::radians(m_Pitch));
            front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            m_Front = glm::normalize(front);
            m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));

            UpdateView();
        }

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;

        float m_Fov;
        float m_Aspect;
        float m_NearPlane;
        float m_FarPlane;

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
    };

}