#include "vkEngine.h"
#include "vkMaterial.h"

namespace vk
{
	vkMaterial::vkMaterial(const vkEngine* pEngine, const std::string& name) : 
		m_pEngine(pEngine), mName(name), mDiffuseTextureIndex(-1), mDiffuseFactor(glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)),
		mSpecularTextureIndex(-1), mSpecularFactor(glm::vec3(1.0f)),
		mEmissiveTextureIndex(-1), mEmissiveFactor(glm::vec3(1.0f)),
		mNormalTextureIndex(-1), mOcculsionTextureIndex(-1),
		mAmbientColor(glm::vec4(0.1f)), mGlossinessFactor(0.5f), bDoubleSided(false)
	{
		mMaterialUBO = {};
		m_MaterialDescriptorSet = VK_NULL_HANDLE;
	}

	vkMaterial::~vkMaterial()
	{}

	void vkMaterial::UpdateMaterialUniformBufferObject()
	{
		mMaterialUBO = { mGlossinessFactor, mDiffuseTextureIndex, mNormalTextureIndex, mDiffuseFactor, mAmbientColor };
	}
}