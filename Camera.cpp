#include "Camera.hpp"
#include "Graphics.hpp"
#include "MathHelper.hpp"

Camera::Camera(float fov, float npd, float fpd, glm::vec3 pos)
	:vertical_field_of_view_degrees_(fov),
	near_plane_distance_(npd),
	far_plane_distance_(fpd),
	position_(pos),
	azimuth_radians_(3.14f),
	inclination_radians_(0.f)
{
	frustum_ = new Frustum(
		fov,
		GFX->GetScreenRatio(),
		fpd,
		npd	
		);
	
}

Camera::~Camera()
{
	delete frustum_;
}

glm::vec3 Camera::Direction()
{	 
	if(!dirty_)
		return direction_;

	//compute direction vector
	const float cos_azi = cosf(azimuth_radians_);
	const float sin_azi = sinf(azimuth_radians_);
	const float cos_inc = cosf(inclination_radians_);
	const float sin_inc = sinf(inclination_radians_);
	direction_ = glm::vec3(cos_inc * -sin_azi, sin_inc, cos_inc * -cos_azi);
	dirty_ = false;
	UpdateFrustum();

	return direction_;
}

void Camera::UpdateFrustum()
{
	frustum_->Update(position_, direction_);
}

void Camera::SetPosition(const glm::vec3& position)
{

	position_ = position;
	UpdateFrustum();
	dirty_ = false;
	
}