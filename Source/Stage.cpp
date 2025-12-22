#include "Stage.h"
#include "../Engine/Fbx.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Model.h"

float addX = -7.0f;
float addZ = -7.0f;

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

	hModel_ = Model::Load("BoxDefault.fbx");

	map_.resize(15);
	Transform check;

	for (int i = 0; i < map_.size(); i++)
	{
		for (int j = 0; j < map_.size(); j++)
		{
			check.position_.x = addX + j * 1.0f;
			check.position_.z = addZ + i * 1.0f;
			map_[i].push_back(1);
			hMapModel_[i * map_.size() + j] = Model::Load("BoxDefault.fbx");
			Model::SetTransform(hMapModel_[i * map_.size() + j], check);
		}
	}
}

void Stage::Update()
{
	transform_.rotate_.x += 0.05f; // フレームワークがない弊害起きてる
	//XMFLOAT4 start; // Rayの始点
	//XMFLOAT4 direction; // Rayの方向（正規化してあること）
	//bool isHit; // 当たったかどうか
	//float distance; // 始点からの距離

	Transform check;
	RayCastData data = {
		XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f),
		XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f),
		false,
		100.0f
	};
	for (int i = 0; i < map_.size(); i++)
	{
		for (int j = 0; j < map_.size(); j++)
		{
			check.position_.x = addX + j * 1.0f;
			check.position_.z = addZ + i * 1.0f;
			check.position_.y = 0.0f;
			XMVECTOR v = { check.position_.x, check.position_.y, check.position_.z };
			XMStoreFloat4(&data.start, v);
			XMVECTOR f = { 1.0f, -1.0f, 1.0f };
			v = v * f;
			XMStoreFloat4(&data.direction, v);
			//モデルを設置位置に移動（トランスフォーム）
	/*		Transform trans;
			trans.position_.x = [モデルのｘ位置];
			trans.position_.y = [モデルのｙ位置];
			trans.position_.z = [モデルのｚ位置];
			Model::SetTransform(hMapModel_[i + map_.size() + j], trans);*/

			Model::RayCast(hMapModel_[i + map_.size() + j], data);
			

			//⑥ レイが当たったらブレークポイントで止める
			if (data.isHit == true)
			{
				//なんか書いてないとifに当たらないから、
				int a = 0;
				a++;
			}
		}
	}
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
