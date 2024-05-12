#include "vkCore.h"
#include "vkMesh.h"
#include "vkFrameObject.h"

namespace vk
{
	vkFrameObject::vkFrameObject() : vkObject("noName", ObjectType::OT_FrameObject),
		mLocalTransformation(glm::mat4(1.0f)), mWorldTransformation(glm::mat4(1.0f)), m_pParentGameObject(nullptr),
		m_pParent(nullptr), m_pNext(nullptr), mNbChildren(0), m_pChildren(nullptr), m_pMesh(nullptr)
	{
	}

	vkFrameObject::vkFrameObject(const std::string& name) : vkObject(name, ObjectType::OT_FrameObject),
		mLocalTransformation(glm::mat4(1.0f)), mWorldTransformation(glm::mat4(1.0f)), m_pParentGameObject(nullptr),
		m_pParent(nullptr), m_pNext(nullptr), mNbChildren(0), m_pChildren(nullptr), m_pMesh(nullptr)
	{
	}

	vkFrameObject::~vkFrameObject()
	{
	}

	void vkFrameObject::AssignMesh(vkMesh* pMesh)
	{
		m_pMesh = pMesh;
	}

	void vkFrameObject::AddMeshPrimitive(const vkPrimitive& primitive)
	{
		m_pMesh->AddPrimitive(primitive);
	}
}