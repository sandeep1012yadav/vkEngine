#include "vkCore.h"
#include "vkFrameObject.h"

namespace vk
{
	vkFrameObject::vkFrameObject() : vkObject("noName", ObjectType::OT_FrameObject)
	{
		mTransformation = glm::mat4(1.0f);
		mWorldTransformation = glm::mat4(1.0f);
		
		m_pParent = nullptr;
		m_pNext = nullptr;

		mNbChildren = 0;
		m_pChildren = nullptr;

		mNbMeshes = 0;
		m_pMeshes = nullptr;
	}

	vkFrameObject::vkFrameObject(const std::string& name) : vkObject(name, ObjectType::OT_FrameObject)
	{
		mTransformation = glm::mat4(1.0f);
		mWorldTransformation = glm::mat4(1.0f);

		m_pParent = nullptr;
		m_pNext = nullptr;

		mNbChildren = 0;
		m_pChildren = nullptr;

		mNbMeshes = 0;
		m_pMeshes = nullptr;
	}

	vkFrameObject::~vkFrameObject()
	{

	}
}