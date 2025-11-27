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
	pFbx_ = new Fbx();
	pFbx_->Load("soil.fbx");
	pSphereCollider_ = new SphereCollider(1.0f);
	AddCollider(pSphereCollider_);

	hModel_ = Model::Load("soil.fbx");
}

void Stage::Update()
{
	transform_.rotate_.y += 0.05f; // フレームワークがない弊害起きてる
}

void Stage::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Stage::Release()
{
	if (pFbx_)
	{
		pFbx_->Release();
		delete pFbx_;
		pFbx_ = nullptr;
	}
}
