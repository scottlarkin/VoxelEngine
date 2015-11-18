#include "FirstPersonController.hpp"
#include "Graphics.hpp"
#include "ChunkManager.hpp"
#include "Clock.hpp"

FirstPersonController::FirstPersonController(ChunkManager* chunkManager, Camera* cam)
	:chunkManager_(chunkManager)
{

	cameraSensitivity = 1.0f;
	invertX = 0;
	invertY = 0;
	
	camera_ = cam;
	position_ = cam->GetPosition();

	

}

FirstPersonController::~FirstPersonController()
{
	delete camera_;
}

void FirstPersonController::Update()
{
	//HandleEvents();
}

void FirstPersonController::HandleEvents(SDL_Event* e)
{
	
	if( e->type ==  SDL_KEYDOWN)
	{
		if(e->key.keysym.sym == SDLK_w)
		{
			MoveForward(0.5);
			chunkManager_->PlayerMoved();
		}
		else if(e->key.keysym.sym == SDLK_s)
		{
			MoveBack(0.5);
			chunkManager_->PlayerMoved();
		}
	}

	else if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
	{
		if( e->type == SDL_MOUSEMOTION)
		{
			Turn(-e->motion.xrel * 0.00174532925f , -e->motion.yrel  * 0.00174532925f );
		}
	}
	 
}

void FirstPersonController::MoveForward(float dist)
{
	camera_->dirty_ = true;
	camera_->SetPosition(camera_->GetPosition() + camera_->Direction() * dist );
	position_ = camera_->GetPosition(); 
}
void FirstPersonController::MoveBack(float dist)
{
	camera_->dirty_ = true;
	camera_->SetPosition(camera_->GetPosition() - camera_->Direction() * dist );
	position_ = camera_->GetPosition();
}	

void FirstPersonController::Turn(float xRads, float yRads)
{
	camera_->dirty_ = true;
	camera_->azimuth_radians_ -= xRads * cameraSensitivity;
	camera_->inclination_radians_ -= yRads * cameraSensitivity;
}