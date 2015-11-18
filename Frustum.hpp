#pragma once
#include "glm/glm.hpp"

class Frustum
{
public:
	
	Frustum(float fov, float ratio, float farDist, float nearDist);
	~Frustum();
	bool PointInFrustum(const glm::vec3& pos);
	bool SphereInFrustum(const glm::vec3& pos, float rad);
	void Update(const glm::vec3& pos,const glm::vec3& dir);

private:

	float nearPlaneDist_;;						
	float farPlaneDist_;;						
	float aspectRatio_;						

	float fov_;							

	float rFactor_;
	float uFactor_;

	float rSphereFactor_;
	float uSphereFactor_;

	glm::vec3 position_;
	glm::vec3 forwardDir_;  //= direction
	glm::vec3 rightDir_;	//= Direction x up
	glm::vec3 upDir_;		// = direction x right
};