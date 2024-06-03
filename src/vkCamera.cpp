#include "vkCamera.h"

namespace vk
{
	vkCamera::vkCamera(const std::string& name) : vkObject(name, ObjectType::OT_Camera), mFov(60.0f), mAspect(16.0f / 9.0f), mNearPlane(0.3f), mFarPlane(10000.0f)
	{
		SetPosition(glm::vec3(0, 0, -1.0f));
		UpdateVectors();
		UpdateViewMatrix();
		SetProjection(mFov, mAspect, mNearPlane, mFarPlane);
	}

	vkCamera::vkCamera(const std::string& name, float fov, float aspect, float nearPlane, float farPlane) : vkObject(name, ObjectType::OT_Camera),
		mFov(fov), mAspect(aspect), mNearPlane(nearPlane), mFarPlane(farPlane)
	{
		SetPosition(glm::vec3(0, 0, -1.0f));
		UpdateVectors();
		UpdateViewMatrix();
		SetProjection(mFov, mAspect, mNearPlane, mFarPlane);
	}

	void vkCamera::SetProjection(float fov, float aspect, float znear, float zfar)
	{
		glm::mat4 currentMatrix = mProjectionMatrix;
		mFov = fov;
		mNearPlane = znear;
		mFarPlane = zfar;
		mProjectionMatrix = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		if (mFlipY) {
			mProjectionMatrix[1][1] *= -1.0f;
		}
	};

	void vkCamera::UpdateAspectRatio(float aspect)
	{
		glm::mat4 currentMatrix = mProjectionMatrix;
		mProjectionMatrix = glm::perspective(glm::radians(mFov), aspect, mNearPlane, mFarPlane);
		if (mFlipY) {
			mProjectionMatrix[1][1] *= -1.0f;
		}
	}

	void vkCamera::Update(float deltaTime)
	{
		if (mMode == Mode::Flycam)
		{
			if (Moving())
			{
				glm::vec3 camFront;
				camFront.x = -cos(glm::radians(mRotation.x)) * sin(glm::radians(mRotation.y));
				camFront.y = sin(glm::radians(mRotation.x));
				camFront.z = cos(glm::radians(mRotation.x)) * cos(glm::radians(mRotation.y));

				mFront = glm::normalize(camFront);
				mRight = glm::normalize(glm::cross(mFront, mWorldUp));
				mUp = glm::normalize(glm::cross(mRight, mFront));

				float moveSpeed = deltaTime * mMovementSpeed;

				if (mKeys.up)
					mPosition += mFront * moveSpeed;
				if (mKeys.down)
					mPosition -= mFront * moveSpeed;
				if (mKeys.left)
					mPosition -= mRight * moveSpeed;
				if (mKeys.right)
					mPosition += mRight * moveSpeed;
			}
		}
		UpdateViewMatrix();
	}

	void vkCamera::UpdateVectors()
	{
		glm::vec3 camFront;
		camFront.x = -cos(glm::radians(mRotation.x)) * sin(glm::radians(mRotation.y));
		camFront.y = sin(glm::radians(mRotation.x));
		camFront.z = cos(glm::radians(mRotation.x)) * cos(glm::radians(mRotation.y));

		mFront = glm::normalize(camFront);
		mRight = glm::normalize(glm::cross(mFront, mWorldUp));
		mUp = glm::normalize(glm::cross(mRight, mFront));
	}
	void vkCamera::UpdateViewMatrix()
	{
		glm::mat4 currentMatrix = mViewMatrix;

		glm::mat4 rotM = glm::mat4(1.0f);
		glm::mat4 transM;

		rotM = glm::rotate(rotM, glm::radians(mRotation.x * (mFlipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 translation = mPosition;
		if (mFlipY) {
			translation.y *= -1.0f;
		}
		transM = glm::translate(glm::mat4(1.0f), translation);

		if (mMode == Mode::Flycam)
		{
			mViewMatrix = rotM * transM;
		}
		else
		{
			mViewMatrix = transM * rotM;
		}
	}
}