#pragma once

#include <string>
#include "Transform.h"
#include <list>

using std::string;
using std::list;

class SphereCollider;

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent, const std::string& name);
	virtual ~GameObject();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;
	void UpdateSub();
	void ReleaseSub();
	void DrawSub(); // 子オブジェクトをすべて描画する

	void SetPosition(XMFLOAT3 position) { transform_.position_ = position; }
	void SetPosition(float x, float y, float z) { transform_.position_.x = x, transform_.position_.y = y, transform_.position_.z = z; }
	void KillMe();

	GameObject* GetRootJob(); 
	GameObject* FindChildObject(const string& name);
	GameObject* FindObject(const string& name);

	void AddCollider(SphereCollider* pCollider);
	void Collision(GameObject* pTarget);
	void RoundRobin(GameObject* pTarget);

	template<class T>
	GameObject* Instantiate(GameObject* parent)
	{
		T* obj = new T(parent);
		obj->Initialize();
		childList_.push_back(obj);
		return obj;
	}

protected:
	list<GameObject*> childList_;
	Transform transform_;
	GameObject* pParent_;
	std::string objectName_;
	SphereCollider* pCollider_;

private:
	bool isDead_;
};