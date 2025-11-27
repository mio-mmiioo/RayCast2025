#pragma once
#include "../Engine/GameObject.h"
#include <vector>

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
	int hMapModel_[15 * 15];
	std::vector<std::vector<int>> map_;

};