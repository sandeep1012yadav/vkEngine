#pragma once
#include "vkCore.h"

namespace vk
{
	class vkEngine;
	class vkMaterial
	{
	public:
		vkMaterial(const vkEngine* pEngine, const std::string& name);
		~vkMaterial();

		std::string mName;
		int32_t mTextureIndex;
		glm::vec4 mDiffuseColor;
		glm::vec4 mAmbientColor;
		glm::vec4 mSpecularColor;
		float mShininess;

	private:
		vkEngine* m_pEngine;
	};
}