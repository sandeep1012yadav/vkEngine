#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
	struct vkPrimitive;
	class vkMesh;
	class vkGameObject;
	class vkFrameObject : public vkObject
	{
	public:
		vkFrameObject();
		explicit vkFrameObject(const std::string& name);
		~vkFrameObject();

		void AssignMesh(vkMesh* pMesh);
		void AddMeshPrimitive(const vkPrimitive& primitive);

		glm::mat4 mLocalTransformation;
		glm::mat4 mWorldTransformation;

		/* GameObject to which this frame is attached. */
		vkGameObject* m_pParentGameObject;

		/** Parent node. NULL if this node is the root node. */
		vkFrameObject* m_pParent;

		/* Next drawable frame to this frame. */
		vkFrameObject* m_pNext;

		/** The number of child nodes of this node. */
		uint32_t mNbChildren;

		/** The child nodes of this node. NULL if mNumChildren is 0. */
		vkFrameObject** m_pChildren;
		
		vkMesh* m_pMesh;

	private:

	};

	

	
}
