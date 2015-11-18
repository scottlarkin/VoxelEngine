#pragma once
#include <SDL.h>
#include "Camera.hpp"
#include "Controller.hpp"

class ChunkManager;

class FirstPersonController: public Controller
{
public:

	FirstPersonController(ChunkManager* chunkManager, Camera* cam);
	~FirstPersonController();

	void Turn(float xRads, float yRads);
	void MoveBack(float dist);
	void MoveForward(float dist);

	void Update();
	void HandleEvents(SDL_Event* e);

private:
	

	float cameraSensitivity;
	bool invertX;
	bool invertY;

	Camera* camera_;
	glm::vec3 position_;

	ChunkManager* chunkManager_;
};