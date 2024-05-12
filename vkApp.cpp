//#include <iostream>
#include "vkEngine.h"
#include "vkScene.h"
#include "vkResourceLoader.h"
using namespace std;


int WinMain()
{
	
	vk::vkEngine* pEngine = new vk::vkEngine();
	vk::vkScene *pScene = pEngine->CreateScene("MainScene");

	pEngine->AddScene(pScene, true);

	pEngine->AddQuadToScene();

	vk::vkResourceLoader::GetInstance()->LoadGameObjectFromGLTF("C:\\MyWorkspace\\VulkanEngine\\SampleData\\adamHead\\adamHead.gltf");

	pEngine->StartEngine();
	pEngine->StopEngine();

	return 0;
}