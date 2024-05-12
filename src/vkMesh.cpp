#include "vkCore.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkMesh.h"

namespace vk
{
	vkMesh::vkMesh(const std::string name) : vkObject(name, ObjectType::OT_Mesh), m_pParentGameObject(nullptr)
	{
		mStartVertexLocation = 0;
		mNbVertices = 0;
		mStartIndexLocation = 0;
		mNbIndices = 0;
	}

	vkMesh::vkMesh() : vkObject("noName", ObjectType::OT_Mesh), m_pParentGameObject(nullptr)
	{
		mStartVertexLocation = 0;
		mNbVertices = 0;
		mStartIndexLocation = 0;
		mNbIndices = 0;
	}

	vkMesh::~vkMesh()
	{
	}

	void vkMesh::AddPrimitive(const vkPrimitive& primitive)
	{
		m_vPrimitives.push_back(primitive);
	}
}