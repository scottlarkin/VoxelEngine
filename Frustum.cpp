#include "Frustum.hpp"
#include "iostream"
#include "Graphics.hpp"

#define ANG2RAD  (3.14159265358979f/180.0f)

Frustum::Frustum(float fov, float ratio, float farDist, float nearDist)
{
	nearPlaneDist_ = nearDist;
	farPlaneDist_ = farDist;
	aspectRatio_ = ratio;
	fov_ = ((ANG2RAD*fov)) * 0.5f;

	position_ = glm::vec3(0,0,0);
	forwardDir_ = glm::vec3(0,0,1);
	rightDir_ = glm::vec3(1,0,0);
	upDir_ = glm::vec3(0,-1,0);

	uFactor_ = std::tan(fov_ * 0.5f);
	rFactor_ = uFactor_ * aspectRatio_;

	uSphereFactor_ = 1.0f / std::cos(fov_ * 0.5f);
	rSphereFactor_ = 1.0f / std::cos(std::atan(rFactor_));

}

Frustum::~Frustum()
{
}	  

void Frustum::Update(const glm::vec3& pos,const glm::vec3& dir)
{
	position_ = pos;
	forwardDir_ = dir;
	rightDir_ = glm::normalize(glm::cross(forwardDir_, glm::vec3(0, -1, 0)));
	upDir_ =  glm::normalize(glm::cross(forwardDir_, rightDir_));

}

bool Frustum::PointInFrustum(const glm::vec3& pos)
{
	glm::vec3 OP = pos - position_;
	
	float forward = glm::dot(OP,forwardDir_);
	if((forward < nearPlaneDist_) || (forward > farPlaneDist_))
		return 0;

	float right = glm::dot(OP,rightDir_);
	float rLimit = rFactor_ * forward;
	if((right < -rLimit) || (right > rLimit))
		return 0;	

		
	float up = glm::dot(OP, upDir_);
	float uLimit = uFactor_ * forward;
	if((up < -uLimit) || (up > uLimit))
		return 0;

	return 1;
}

bool Frustum::SphereInFrustum(const glm::vec3& pos, float rad)
{
	glm::vec3 OP = pos - position_;

	float forward = glm::dot(OP,forwardDir_);
	if((forward < nearPlaneDist_ - rad) || (forward > farPlaneDist_ + rad))
		return 0;

	float d = rad * rSphereFactor_;
	float right = glm::dot(OP, rightDir_);
	float rLimit = rFactor_ * forward + d;
	if(right < -rLimit || right > rLimit)
		return 0;

	d = rad * uSphereFactor_;
	float up = glm::dot(OP, upDir_);
	float uLimit = uFactor_ * forward + d;
	if(up < -uLimit || up > uLimit)
		return 0;


	return 1;

}