#pragma once
#include "vkCore.h"
#include <fstream>
namespace vk
{
	class vkLogger
	{
	public:
		~vkLogger();
		static vkLogger* GetInstance();
		template<typename... Param>
		void Log(const Param&... param)
		{
			std::stringstream ss;
			ToStringStream(ss, param...);
			m_LogFileObject << ss.str() << std::endl;
		}
	private:
		vkLogger();
		static std::unique_ptr<vkLogger> m_Instance;
		static std::ofstream m_LogFileObject;
		
		template<typename T>
		void ToStringStream(std::stringstream& stream, const T& start)
		{
			stream << start;
		}

		template<typename T, typename... Targs>
		void ToStringStream(std::stringstream& stream, const T& start, const Targs&... args)
		{
			stream << start;
			ToStringStream(stream, args...);
		}
	};


#define vkLog vkLogger::GetInstance()
}


