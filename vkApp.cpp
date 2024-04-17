//#include <iostream>
#include "vkEngine.h"
#include "vkScene.h"
using namespace std;


int WinMain()
{
	
	vk::vkEngine* pEngine = new vk::vkEngine();
	vk::vkScene *pScene = pEngine->CreateScene("MainScene");

	pEngine->AddScene(pScene, true);

	pEngine->AddQuadToScene();

	pEngine->StartEngine();
	pEngine->StopEngine();

	return 0;
}