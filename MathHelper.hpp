
#include "glm/glm.hpp"

//generate transformation matrices for glm... profing shows improvements
//when using translate over glm::translate

inline const glm::mat4x4 translate(float x, float y, float z)
{
	return glm::mat4x4(
		1,       0,       0,       0,
		0,       1,       0,       0,
		0,       0,       1,       0,
		x,       y,       z,       1);
}

inline const glm::mat4x4 translate(const glm::vec3& v)
{
	return glm::mat4x4( 
		1,       0,       0,       0,
		0,       1,       0,       0,
		0,       0,       1,       0,
		v.x,       v.y,       v.z,       1);
}

inline glm::mat4x4 rotateX(float r)
{
	return glm::mat4x4( 
		1,       0,       0,       0,
		0,       cos(r),       -sin(r),       0,
		0,       sin(r),       cos(r),       0,
		0,       0,       0,       1);
}

inline const glm::mat4x4 rotateY(float r)
{
	return glm::mat4x4(   
		cos(r),       0,       sin(r),       0,
		0,       1,       0,       0,
		-sin(r),       0,       cos(r),       0,
		0,       0,      0,       1);
}

inline const glm::mat4x4 rotateZ(float r)
{
	return glm::mat4x4( 
		cos(r),       -sin(r),       0,       0,
		sin(r),       cos(r),       0,       0,
		0,       0,       1,       0,
		0,       0,       0,       1);
}

inline const glm::mat4x4 scale(float x, float y, float z)
{
	return glm::mat4x4(
		x,       0,       0,       0,
		0,       y,       0,       0,
		0,       0,       z,       0,
		0,       0,       0,       1);
}

inline const glm::mat4x4 scale(float s)
{

	return glm::mat4x4(
		s,       0,       0,       0,
		0,       s,       0,       0,
		0,       0,       s,       0,
		0,       0,       0,       1);
}

inline const float deg2Rad(float degrees)
{
	//radians = degrees * (pi / 180)
	return degrees * 0.0174532925f;
}

//Stolen from Quake3 source
inline const float fastSquareRoot(float number) {
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}

//I dont trust glm::vec3::length() ...
inline const float vec3Magnitude(const glm::vec3& vec)
{
	return fastSquareRoot(
			(vec.x*vec.x) +
			(vec.y*vec.y) +
			(vec.z*vec.z)
		   );
}

inline const float distanceBetween(const glm::vec3& v1, const glm::vec3& v2 )
{
	return vec3Magnitude(v1-v2);
}

//return true if d2 is closer to d1, than d3. false if d3 is closer to d1
inline const bool DistanceCompare(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3 )
{
	float d1, d2;
	glm::vec3 vec = v1 - v2;
	d1 = (vec.x*vec.x) +(vec.y*vec.y) + (vec.z*vec.z);
	vec = v1 - v3;
	d2 = (vec.x*vec.x) +(vec.y*vec.y) + (vec.z*vec.z);

	return d1 < d2;
}