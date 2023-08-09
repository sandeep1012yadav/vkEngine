#include "vkPipeline.h"
#include "vkEngine.h"
#include "vkDevice.h"

namespace vk
{
	vkPipeline::vkPipeline(vkDevice* pDevice, vkEngine* pEngine)
	{
		m_pDevice = pDevice;
		m_pEngine = pEngine;
	}

	vkPipeline::~vkPipeline()
	{
	}
}

