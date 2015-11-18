

#pragma once


#include "Entity.hpp"

class TestEntity: public Entity
{

public:
	TestEntity(){}
	~TestEntity(){}

	void initialize(Vector2& pos)
	{
		setImgName(IMG_RED_CIRCLE);
		setPos(pos);
		setSpeed(1);
		setDirection(Vector2(1,1));
	}

	void update(){}



};
