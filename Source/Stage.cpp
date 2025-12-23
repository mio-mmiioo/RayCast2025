#include "Stage.h"
#include "../Engine/Fbx.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Model.h"
#include "../resource.h"

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

BOOL Stage::LocalProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: //コントロールの操作
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL Stage::ManuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), BM_SETCHECK, BST_CHECKED, (LPARAM)("デフォルト"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)("レンガ"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)("草地"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)("砂地"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)("水場"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, 0);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO1:
			mode_ = 0; // 上げ
			return true;
		case IDC_RADIO2:
			mode_ = 1; // 下げ
			return true;
		case IDC_RADIO3:
			mode_ = 2; // 種類変更
			return true;
		case IDC_COMBO1:
			select_ = (int)SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);
			return true;
		}
		return false;
	}
	return false;
}
