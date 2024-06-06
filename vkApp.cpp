//#include <iostream>
#include "vkEngine.h"
#include "vkScene.h"
#include "vkResourceLoader.h"
//#include "vkGameObject.h"
using namespace std;


int WinMain()
{
	
	vk::vkEngine* pEngine = new vk::vkEngine();
	vk::vkScene *pScene = pEngine->CreateScene("MainScene");

	//vk::vkGameObject* pGameObject = pEngine->CreateQuad();
	
	vk::vkGameObject* pGameObject = vk::vkResourceLoader::GetInstance()->LoadGameObjectFromGLTF("./sample_data/FlightHelmet/FlightHelmet.gltf");
	
	pScene->AddGameObject(pGameObject);
	pEngine->AddScene(pScene, true);

	pEngine->StartEngine();
	pEngine->StopEngine();

	return 0;
}