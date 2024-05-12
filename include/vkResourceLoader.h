#pragma once
#include "vkCore.h"

namespace tinygltf
{
	class Node;
	class Model;
};

namespace vk
{
	struct vkVertex;
	class vkEngine;
	class vkTexture;
	class vkMesh;
	class vkFrameObject;
	class vkGameObject;
	class vkResourceLoader
	{
	public:
		static vkResourceLoader* GetInstance();
		void LoadGameObjectFromGLTF(const std::string fileName);
		~vkResourceLoader();

	private:
		static vkResourceLoader* m_pResourceLoader;
		static vkResourceLoader* CreateInstance(const vkEngine* pEngine);
		const vkEngine* m_pEngine;
		vkResourceLoader(const vkEngine* pEngine);

		//gltf loading functions
		vkFrameObject* LoadGLTFNode(const tinygltf::Node& node, const tinygltf::Model& model,
			std::vector<vkVertex>& vVertices, std::vector<uint32_t>& vIndices);
		void LoadGLTFMaterails(const tinygltf::Model& model, std::map<uint32_t, uint32_t>& materialIdMap);
		void LoadGLTFTextures(const tinygltf::Model& model, std::map<uint32_t, uint32_t>& textureIdMap);

		friend class vkEngine;
	};

	
}