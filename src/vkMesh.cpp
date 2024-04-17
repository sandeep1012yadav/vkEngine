#include "vkCore.h"
#include "vkMesh.h"

namespace vk
{
	vkMesh::vkMesh(const std::string name) : vkObject(name, ObjectType::OT_Mesh)
	{
		mNbVertices = 0;
		m_pVertices = nullptr;
		mNbIndices = 0;
		m_pIndices = nullptr;
		mStartVertexLocation = 0;
		mStartIndexLocation = 0;
	}

	vkMesh::vkMesh() : vkObject("noName", ObjectType::OT_Mesh)
	{
		mNbVertices = 0;
		m_pVertices = nullptr;
		mNbIndices = 0;
		m_pIndices = nullptr;
		mStartVertexLocation = 0;
		mStartIndexLocation = 0;
	}

	vkMesh::~vkMesh()
	{
		delete m_pVertices;
		delete m_pIndices;
	}
}