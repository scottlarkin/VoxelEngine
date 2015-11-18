#pragma once
#include "Entity.hpp"

#define INVISIBLE 0
#define GRASS 1

class Block: public Entity
{
  public:

	Block(){}
	~Block(){}

	const bool IsVisible(){return type_ != INVISIBLE;}
	
	void SetVisible(bool v){type_ = v ? GRASS : INVISIBLE;}

	char getType(){return type_;}

private:

	//store types as char because it only uses 1 byte of memory
	char type_;

};