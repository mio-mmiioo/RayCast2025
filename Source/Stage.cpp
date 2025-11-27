#include "Stage.h"
#include "../Engine/Fbx.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Model.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Player"),
	pFbx_(nullptr),
	pSphereCollider_(nullptr),
	hModel_(-1)
{
	
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	//pFbx_ = new Fbx();
	//pFbx_->Load("tamesi.fbx");
	pSphereCollider_ = new SphereCollider(1.0f);
	AddCollider(pSphereCollider_);

	hModel_ = Model::Load("empty.fbx");

	map_.resize(15);
	Transform check;
	float addX = -7.0f;
	float addZ = -7.0f;
	for (int i = 0; i < map_.size(); i++)
	{
		for (int j = 0; j < map_.size(); j++)
		{
			check.position_.x = addX + j * 1.0f;
			check.position_.z = addZ + i * 1.0f;
			map_[i].push_back(1);
			hMapModel_[i * map_.size() + j] = Model::Load("empty.fbx");
			Model::SetTransform(hMapModel_[i * map_.size() + j], check);
		}
	}
}

void Stage::Update()
{
	//transform_.rotate_.x += 0.05f; // フレームワークがない弊害起きてる
}

void Stage::Draw()
{
	//Model::SetTransform(hModel_, transform_);
	//Model::Draw(hModel_);
	float x = 0.0f;
	float z = 0.0f;

	for (int i = 0; i < map_.size(); i++)
	{
		for (int j = 0; j < map_.size(); j++)
		{
			if (map_[i][j] == 1)
			{
				//transform_.position_.x = x;
				//transform_.position_.z = z;
				//Model::SetTransform(hMapModel_[i * (j + 1) + j], transform_);
				Model::Draw(hMapModel_[i * map_.size() + j]);
			}
		}
	}
}

void Stage::Release()
{
	//if (pFbx_)
	//{
	//	pFbx_->Release();
	//	delete pFbx_;
	//	pFbx_ = nullptr;
	//}
}
