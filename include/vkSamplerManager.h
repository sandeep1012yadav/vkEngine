#pragma once
#include "vkCore.h"

namespace vk
{
	class vkEngine;
    class vkDevice;
	class vkSamplerManager
	{
	public:
		static vkSamplerManager* GetInstance();
        ~vkSamplerManager();
		VkSampler GetSampler(const VkSamplerCreateInfo& samplerInfo);

	private:
		static vkSamplerManager* m_pSamplerManager;
		const vkEngine* m_pEngine;
        const vkDevice* m_pDevice;
        std::unordered_map<size_t, VkSampler> m_Samplers;

		vkSamplerManager(const vkEngine* pEngine);
		static void CreateInstance(const vkEngine* pEngine);
		static void DestroyInstance();
		size_t HashCombine(size_t lhs, size_t rhs);
		size_t HashSamplerCreateInfo(const VkSamplerCreateInfo& samplerInfo);

		friend class vkEngine;
	};	
}