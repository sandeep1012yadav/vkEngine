#pragma once
#include "vkCore.h"

namespace vk
{
	class vkEngine;
	class vkDevice;
	class vkPipeline
	{
	public:
		vkPipeline(vkDevice* pDevice, vkEngine* pEngine);
		~vkPipeline();

	private:
		vkEngine* m_pEngine;
		vkDevice* m_pDevice;
		VkPipeline m_vkPipeline;
	};
}