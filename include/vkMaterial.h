#pragma once
#include "vkCore.h"

namespace vk
{
	struct MaterialUniformBufferObject
	{
		float mGlossinessFactor;
		int32_t mDiffuseTextureIndex;
		int32_t mNormalTextureIndex;
		glm::vec4 mDiffuseFactor;
		glm::vec4 mAmbientColor;
	};

	struct BufferImageDescriptorSetResources
	{
		VkDescriptorBufferInfo bufferInfo = { VK_NULL_HANDLE, 0, 0 };
		VkDeviceMemory		   bufferMemory = VK_NULL_HANDLE;
		VkDescriptorImageInfo  imageInfo = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED };
		VkDeviceMemory         imageMemory = VK_NULL_HANDLE;
	};

	class vkEngine;
	class vkMaterial
	{
	public:
		vkMaterial(const vkEngine* pEngine, const std::string& name);
		~vkMaterial();

		std::string mName;
		
		int32_t mDiffuseTextureIndex;
		glm::vec4 mDiffuseFactor;

		int32_t mSpecularTextureIndex;
		glm::vec3 mSpecularFactor;

		int32_t mEmissiveTextureIndex;
		glm::vec3 mEmissiveFactor;

		int32_t mNormalTextureIndex;
		int32_t mOcculsionTextureIndex;

		glm::vec4 mAmbientColor;

		float mGlossinessFactor;
		bool bDoubleSided;

		MaterialUniformBufferObject mMaterialUBO;
		VkDescriptorSet m_MaterialDescriptorSet;
		BufferImageDescriptorSetResources mMaterialDescriptorSetResources;

		void UpdateMaterialUniformBufferObject();
		const VkDescriptorSet& GetMaterialDescriptorSet() const { return m_MaterialDescriptorSet; }

	private:
		const vkEngine* m_pEngine;
	};
}