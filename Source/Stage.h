#pragma once
#include "../Engine/GameObject.h"

class Fbx;
class SphereCollider;

class Stage : public GameObject
{
public:
	Stage(GameObject* parent);
	~Stage();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	Fbx* pFbx_;
	SphereCollider* pSphereCollider_;
	int hModel_;
};