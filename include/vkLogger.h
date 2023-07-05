#pragma once
#include <string>
#include <fstream>
namespace vk
{
	class vkLogger
	{
	public:
		~vkLogger();
		static vkLogger* GetInstance();
		void Log(const std::string& _log);
	private:
		vkLogger();
		static vkLogger* m_Instance;
		static std::ofstream m_LogFileObject;
	};


#define vkLog vkLogger::GetInstance()
}


