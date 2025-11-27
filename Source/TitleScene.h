#pragma once
#include "../Engine/GameObject.h"

class TitleScene : public GameObject
{
public:
	TitleScene(GameObject* parent);
	~TitleScene();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};