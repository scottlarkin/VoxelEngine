#include "World.hpp"

#include <iostream>
#include <cstdlib>
#include <thread>

World::World(ChunkManager* chunkManager)
{
	chunkManager_ = chunkManager;

}

World::~World()
{
	delete chunkManager_;
	
}

void World::Render()
{
	chunkManager_->RenderChunks();	
}

void World::Update()
{
	chunkManager_->UpdateLists();
}


