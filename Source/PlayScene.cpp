#include "PlayScene.h"
#include "../Engine/Input.h"
#include "../Engine/GameObject.h"
#include "../Engine/SceneManager.h"
#include "Stage.h"

PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent, "PlayScene")
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	Instantiate<Stage>(this);
}

void PlayScene::Update()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_TITLE);
	}
}

void PlayScene::Draw()
{
}

void PlayScene::Release()
{
}