//
//Scott Larkin, 2015


#include "Graphics.hpp"
#include <SDL_ttf.h>
#include "Clock.hpp"
#include <iostream>
#include "World.hpp"
#include <thread>
#include "FirstPersonController.hpp"



int main( int argc, char* args[] )
{
	bool quit = false;
	SDL_Event e;
	GFX->Init(1300,700);
	Graphics* h = GFX->GetInstance();

    SDL_SetRelativeMouseMode(SDL_TRUE);

	Camera* camera = new Camera(75,5,200,glm::vec3(0,-64,0));

	GFX->SetCamera(camera);

	//Clock* clock = new Clock();					
	ChunkManager* chunkManager = new ChunkManager();

	FirstPersonController* fpc = new FirstPersonController(chunkManager, camera);
	World* world = new World(chunkManager);
			
	while(!quit)
	{
		//handle inputs
		while( SDL_PollEvent( &e ) != 0 )
        { 
			fpc->HandleEvents(&e);

			if( e.type == SDL_QUIT )
			{
			    quit = true;
			}
			else if(  e.type ==  SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
			}
		}
		
		//update world

		//CLK->Update();
		world->Update();
		//render graphics

		//prepare back buffer
		GFX->PreRender();
		//render visible entities to backbuffer
		world->Render();
		//Swap screen buffers

		//GFX->RenderText(0,0,"hello world");
		GFX->SwapBuffer();

	}
			
	delete fpc;
	//delete clock;
	delete world;
	delete chunkManager;

	return 0;
}


