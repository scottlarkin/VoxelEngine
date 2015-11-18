#pragma once
#include "glm\glm.hpp"
#include "Frustum.hpp"

class Camera
{
public:
	Camera(float fov, float npd, float fpd, glm::vec3 pos);
	~Camera();

	Frustum* GetFrustum()const{return frustum_;}

	void UpdateFrustum();

    
    float vertical_field_of_view_degrees_;
    float near_plane_distance_;
    float far_plane_distance_;
	float azimuth_radians_;
	float inclination_radians_;
	bool dirty_;

	glm::vec3 Direction();
	const glm::vec3& GetPosition()const {return position_;}

	void SetPosition(const glm::vec3& positionn);

private:
	glm::vec3 position_;
	glm::vec3 direction_;
	Frustum* frustum_;
};