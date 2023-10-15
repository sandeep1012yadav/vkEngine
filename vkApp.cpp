//#include <iostream>
#include "vkEngine.h"

using namespace std;


int WinMain()
{
	string s = "*******************************************************\n****************Welcome To Vulkan Engine***************\n*******************************************************\n*******************************************************";
	vk::vkLog->Log(s);
	vk::vkEngine* pEngine = new vk::vkEngine();

	pEngine->StartEngine();
	pEngine->StopEngine();

	return 0;
}