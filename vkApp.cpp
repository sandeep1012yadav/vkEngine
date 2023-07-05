//#include <iostream>
#include "vkEngine.h"

using namespace std;


int WinMain()
{
	//cout << "Welcome To Vulkan Engine";

	vk::vkEngine* pEngine = new vk::vkEngine();

	pEngine->StartEngine();
	pEngine->StopEngine();

	return 0;
}