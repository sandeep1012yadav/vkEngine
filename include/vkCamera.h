#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
	class vkCamera : public vkObject
	{
	private:

		float mFov;
		float mAspect;
		float mNearPlane;
		float mFarPlane;

		glm::mat4 mViewMatrix;
		glm::mat4 mProjectionMatrix;

		void UpdateVectors();
		void UpdateViewMatrix();
		
	public:
		enum class Mode {
			Flycam,
			Arcball
		};
		Mode mMode = Mode::Flycam;


		vkCamera(const std::string& name);
		vkCamera(const std::string& name, float fov, float aspect, float nearPlane, float farPlane);

		glm::vec3 mRotation = glm::vec3();
		glm::vec3 mPosition = glm::vec3();
		glm::vec4 mViewPos = glm::vec4();

		glm::vec3 mFront = glm::vec3();
		glm::vec3 mUp = glm::vec3();
		glm::vec3 mRight = glm::vec3();
		glm::vec3 mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float mRotationSpeed = 1.0f;
		float mMovementSpeed = 1.0f;

		//bool mUpdated = true;
		bool mFlipY = true;

		struct
		{
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;
		} mKeys;

		bool Moving()
		{
			return mKeys.left || mKeys.right || mKeys.up || mKeys.down;
		}

		float GetNearClip() { 
			return mNearPlane; }

		float GetFarClip() {
			return mFarPlane;
		}

		const glm::mat4& GetProjectionMatrix()
		{
			return mProjectionMatrix;
		}

		const glm::mat4& GetViewMatrix()
		{
			return mViewMatrix;
		}

		void SetMode(Mode mode)
		{
			mMode = mode;
		}

		Mode GetMode()
		{
			return mMode;
		}

		void SetPosition(glm::vec3 position)
		{
			mPosition = position;
			UpdateViewMatrix();
		}

		void SetRotation(glm::vec3 rotation)
		{
			mRotation = rotation;
			UpdateViewMatrix();
		}

		void Rotate(glm::vec3 delta)
		{
			mRotation += delta;
			UpdateViewMatrix();
		}

		void SetTranslation(glm::vec3 translation)
		{
			mPosition = translation;
			UpdateViewMatrix();
		};

		void Translate(glm::vec3 delta)
		{
			mPosition += delta;
			UpdateViewMatrix();
		}

		void SetRotationSpeed(float rotationSpeed)
		{
			mRotationSpeed = rotationSpeed;
		}

		void SetMovementSpeed(float movementSpeed)
		{
			mMovementSpeed = movementSpeed;
		}

		void Update(float deltaTime);
		void SetProjection(float fov, float aspect, float znear, float zfar);
		void UpdateAspectRatio(float aspect);
	};

}