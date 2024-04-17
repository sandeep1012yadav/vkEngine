#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
	class vkFrameObject : public vkObject
	{
	public:
		vkFrameObject();
		explicit vkFrameObject(const std::string& name);
		~vkFrameObject();

		glm::mat4 mTransformation;
		glm::mat4 mWorldTransformation;

		/** Parent node. NULL if this node is the root node. */
		vkFrameObject* m_pParent;

		/* Next drawable frame to this frame. */
		vkFrameObject* m_pNext;

		/* GameObject to which this frame is attached. */
		//GameObject* pGameObject;

		/** The number of child nodes of this node. */
		uint32_t mNbChildren;

		/** The child nodes of this node. NULL if mNumChildren is 0. */
		vkFrameObject** m_pChildren;

		/** The number of meshes of this node. */
		uint32_t mNbMeshes;

		/** The meshes of this node. Each entry is an index into the
		  * mesh list of the resource pool.
		  */
		uint32_t* m_pMeshes;



	private:

	};

	

	
}
