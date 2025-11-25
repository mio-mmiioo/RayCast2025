#include "GameObject.h"
#include "SphereCollider.h"
#include "MessageDispatcherApi.h"

GameObject::GameObject()
	:pParent_(nullptr),
	isDead_(false)
{
}

GameObject::GameObject(GameObject* parent, const std::string& name)
	:pParent_(parent), objectName_(name), isDead_(false)
{
	if (parent != nullptr)
	{
		transform_.pParent_ = &(parent->transform_);
	}
}

GameObject::~GameObject()
{
}

void GameObject::Release()
{
}

void GameObject::UpdateSub()
{
	Update();

	RoundRobin(GetRootJob());

	for (auto child : childList_)
	{
		child->UpdateSub();
	}

	for (auto itr = childList_.begin(); itr != childList_.end();)
	{
		if ((*itr)->isDead_)
		{
			(*itr)->ReleaseSub();
			delete(*itr);
			itr = childList_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void GameObject::ReleaseSub()
{
	this->Release(); 
	for (auto child : childList_)
	{
		child->ReleaseSub();
	}
}

void GameObject::DrawSub()
{
	// 自身を描画
	// 子オブジェクトを描画 childList の各要素に対してDrawを呼ぶ

	Draw();
	for (auto child : childList_)
	{
		child->DrawSub();
	}
}

void GameObject::KillMe()
{
	isDead_ = true;
}

GameObject* GameObject::GetRootJob()
{
	if (pParent_ == nullptr)
	{
		return this; // 親がいない→RootJob
	}
	else
	{
		return pParent_->GetRootJob();
	}
}

GameObject* GameObject::FindChildObject(const string& name)
{
	if (objectName_ == name)
	{
		return this; // 探されているオブジェクト
	}
	else
	{
		for (auto child : childList_)
		{
			GameObject* result = child->FindChildObject(name);
			if (result != nullptr)
			{
				return result;
			}
		}
		return nullptr; // 見つからなかった
	}
}

GameObject* GameObject::FindObject(const string& name)
{
	// AIに聞いた方
	GameObject* rootJob = GetRootJob();
	if (rootJob == nullptr)
	{
		return rootJob;
	}
	return rootJob->FindChildObject(name);
}

void GameObject::AddCollider(SphereCollider* pCollider)
{
	pCollider_ = pCollider;
}

void GameObject::Collision(GameObject* pTarget)
{
	// this->pCollider_とpTarget->pCollider_がぶつかっているか否か
	float thisR = this->pCollider_->GetRadius();
	float tgtR = pTarget->pCollider_->GetRadius();
	float thre = (thisR + tgtR) * (thisR + tgtR);
	// 2つのコライダーの距離を計算する
	XMFLOAT3 thisP = this->transform_.position_;
	XMFLOAT3 tgtP = pTarget->transform_.position_;
	float distance = (thisP.x - tgtP.x) * (thisP.x - tgtP.x) +
		(thisP.y - tgtP.y) * (thisP.y - tgtP.y) +
		(thisP.z - tgtP.z) * (thisP.z - tgtP.z);
	// コライダー同士が交差していたら
	if (distance <= thre) {
		MessageBoxA(0, "ぶつかった", "collider", MB_OK);
	}
}

void GameObject::RoundRobin(GameObject* pTarget)
{
	if (pTarget->pCollider_ == pCollider_)
	{
		return;
	}

	// 自分にコライダーがなかったらリターン
	if (pCollider_ == nullptr)
	{
		return;
	}
	// 自分とターゲット自体のコライダーの当たり判定
	if (pTarget->pCollider_ != nullptr)
	{
		Collision(pTarget);
	}
	// 再帰的な奴で、ターゲットの子オブジェクトを当たり判定してく

	for (auto itr : pTarget->childList_)
	{
		RoundRobin(itr);
	}
}
