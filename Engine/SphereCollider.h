#pragma once

class SphereCollider
{
public:
	SphereCollider(float radius);
	~SphereCollider();

	float GetRadius() { return radius_; }

private:
	float radius_;
};