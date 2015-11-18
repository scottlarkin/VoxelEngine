#pragma once
#include <vector>
#include "ChunkManager.hpp"

class World
{
public:
	World(ChunkManager* chunkManager);
	~World();
	void Update();
	void Render();
	void HideInvisibleBlocks();
	int totalSegmentSize_;

	
	
private:
	
	int SegmentSize_;
	int ChunkSize_;

	float gravity_;

	int range_;
	ChunkManager* chunkManager_;

};
