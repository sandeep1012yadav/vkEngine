#include "vkEngine.h"
#include "vkMaterial.h"

namespace vk
{
	vkMaterial::vkMaterial(const vkEngine* pEngine, const std::string& name) : m_pEngine(), mName(name), mTextureIndex(-1), mDiffuseColor(glm::vec4(1.0f)),
		mAmbientColor(glm::vec4(1.0f)), mSpecularColor(glm::vec4(1.0f)), mShininess(0.5)
	{
	}

	vkMaterial::~vkMaterial()
	{
	}
}