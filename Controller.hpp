#pragma once

class Controller
{
public:
	virtual void Turn(float xRads, float yRads) = 0;
	virtual void MoveBack(float dist) = 0;
	virtual void MoveForward(float dist) = 0;

	virtual void Update()= 0;

};





