#include "Transform.h"

Transform::Transform()
{
	matTranslate_ = XMMatrixIdentity();
	matRotate_ = XMMatrixIdentity();
	matScale_ = XMMatrixIdentity();

	pParent_ = nullptr;

	position_ = { 0, 0, 0 };
	rotate_ = { 0, 0, 0 };
	scale_ = { 1, 1, 1 };

}

Transform::~Transform()
{
}

void Transform::Calculation()
{
	matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z); //ˆÚ“®
	matRotate_ = XMMatrixRotationRollPitchYaw(rotate_.x, rotate_.y, rotate_.z); //‰ñ“]
	matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);					//Šgk
}

XMMATRIX Transform::GetWorldMatrix()
{
	Calculation();

	if (pParent_ != nullptr)
	{
		return  matScale_ * matRotate_ * matTranslate_  * pParent_->GetWorldMatrix();
	}
	else
	{
		return  matScale_ * matRotate_ * matTranslate_;
	}
}

XMMATRIX Transform::GetNormalMatrix()
{
	return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}