#include "vkLogger.h"

namespace vk
{
	std::unique_ptr<vkLogger> vkLogger::m_Instance(nullptr);
	std::ofstream vkLogger::m_LogFileObject;

	vkLogger::vkLogger()
	{
	}

	vkLogger::~vkLogger()
	{
		m_LogFileObject.close();
	}

	vkLogger* vkLogger::GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance.reset(new vkLogger());
			m_LogFileObject.open("Log.txt", std::ios::out);
		}

		return m_Instance.get();
	}

	

	
}