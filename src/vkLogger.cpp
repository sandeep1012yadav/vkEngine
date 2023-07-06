#include "vkLogger.h"
namespace vk
{
	vkLogger* vkLogger::m_Instance = nullptr;
	std::ofstream vkLogger::m_LogFileObject;
	vkLogger::vkLogger()
	{
	}

	vkLogger::~vkLogger()
	{
		m_LogFileObject.close();
		delete m_Instance;

	}

	vkLogger* vkLogger::GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new vkLogger();
			m_LogFileObject.open("Log.txt", std::ios::out | std::ios::app);
		}

		return m_Instance;
	}

	void vkLogger::Log(const std::string& _log)
	{
		m_LogFileObject << _log << std::endl;
	}
}