#include "ChunkManager.hpp"
#include "Graphics.hpp"						  
#include "MathHelper.hpp"
#include <algorithm>
#include <queue>

Thread_Safe_Queue<Chunk*> ChunkManager::loadedList_;
Thread_Safe_Queue<Chunk*> ChunkManager::toLoadList_;
std::unordered_map<glm::vec3, Chunk*, hashFunc> ChunkManager::chunkMap_;
bool ChunkManager::finished_ = 0;
int ChunkManager::threadCount_ = 10;
bool ChunkManager::playerMoved_ = 0;
std::vector<ConcurrentMeshData*> ChunkManager::meshStorage_;
std::condition_variable ChunkManager::condition_;
int ChunkManager::maxChunkDist_;

ChunkManager::ChunkManager()  
{

	glm::vec3 pos(0,0,0);
	const Camera* cam = GFX->GetCamera();
	glm::vec3 camPos = cam->GetPosition();
	int dist = (1.5*cam->far_plane_distance_) / Chunk::chunkSize_;

	maxChunkDist_ = (1.5*cam->far_plane_distance_) + (Chunk::chunkSize_/2);

	//load cubes in a sphere around the camera
	for(int i = -dist; i < dist; i++)
	{
		for(int j = -dist; j < dist; j++)
		{
			for(int k = -dist; k < dist; k++)
			{
				//if position is within view dist
				if(distanceBetween(camPos,
					glm::vec3(i*Chunk::chunkSize_ + (Chunk::chunkSize_/2),j*Chunk::chunkSize_+ (Chunk::chunkSize_/2),k*Chunk::chunkSize_+ (Chunk::chunkSize_/2)) + camPos )
					<= maxChunkDist_)
				{
					Chunk* c = new Chunk(glm::vec3(i*Chunk::chunkSize_,j*Chunk::chunkSize_,k*Chunk::chunkSize_) + camPos,
						GFX->AllocateMesh());

					chunks_.push_back(c);

					chunkMap_.insert(std::make_pair(c->GetPosition(), c));

					toLoadList_.push(c);
				}

			}
		}
	}

	toLoadList_.Sort([&](const Chunk* a, const Chunk* b)
	{
		//sort list by distance of chunk from camera
		return DistanceCompare(camPos, b->GetCenter(), a->GetCenter());
	});

	//LOADER THREADS
	for(int i = 0; i < threadCount_; i++)
	{
		//delete this!
		ConcurrentMeshData* meshData = new ConcurrentMeshData(meshStorage_.size(), Chunk::chunkSize_);
		meshStorage_.push_back(meshData);

		//thead to do all mesh computation for a chunk, then add it to the loaded list for the OpenGL thread to finish loading
		workerThreads_.push_back(std::thread([]()
		{
			int meshStorageID = meshStorage_.size() -1;
			ConcurrentMeshData* thisMeshData = meshStorage_[meshStorageID];

			while(!finished_)
			{	
				//pause thread when not in use.	(unpause with meshstorage.unlock)
				std::unique_lock<std::mutex> locker(thisMeshData->GetMutex());
				thisMeshData->Wait(locker); //<-- redundancy here... the func can already see the mutex no need to pass it #WhatWasIThinking
				thisMeshData->Lock();

				Chunk* c = toLoadList_.pop();

				if(c)
				{
					c->Load(thisMeshData, meshStorageID);

					loadedList_.push(c);
				}
				else
				{
					thisMeshData->SetReady(1);
				}
			}
			threadCount_--;
		}));

		workerThreads_.back().detach();
	}

	//This thread looks for chunks that are out of range, moves them to a new position in range, and adds them to load queue
	std::thread t([]()
	{
		std::mutex lock;

		const Camera* cam = GFX->GetCamera();
		glm::ivec3 camPosOld = Pos2ChunkPos(cam->GetPosition());
		glm::ivec3 camPosNew;
		std::queue<Chunk*> readyToLoadChunks;
		int dist = (1.5*cam->far_plane_distance_)/ Chunk::chunkSize_ ;

		while(true)
		{
			//pause thread when not in use (player movement triggers thread to start)
			std::unique_lock<std::mutex> locker(lock);
			condition_.wait(locker, [=]
				{return playerMoved_;}
			);

			camPosNew = Pos2ChunkPos(cam->GetPosition());

			if(camPosNew != camPosOld)//cam has moved into a new chunk. 
			{
				//find any chunks outside view range
				for(auto it = chunkMap_.begin(); it != chunkMap_.end(); ++it)
				{
					if(distanceBetween(it->second->GetCenter(), camPosNew) > maxChunkDist_)
					{
						readyToLoadChunks.push(it->second);
					}
				}

				for(int i = -dist; i < dist; i++)
				{
					for(int j = -dist; j < dist; j++)
					{
						for(int k = -dist; k < dist; k++)
						{
							glm::ivec3 pos = glm::ivec3(i*Chunk::chunkSize_,j*Chunk::chunkSize_,k*Chunk::chunkSize_) + camPosNew;

							if(distanceBetween(pos, camPosNew) <= maxChunkDist_ && //position is within sphere
								chunkMap_.find(pos) == chunkMap_.end()			&& //position is not in map
								readyToLoadChunks.size() > 0)	     			   //chunk is available
							{

								Chunk* c = readyToLoadChunks.front();
								readyToLoadChunks.pop();				 //take chunk from ready to load list

								chunkMap_.erase(c->GetPosition());	     //remove old position from map
								c->SetPosition(pos);
								chunkMap_.insert(std::make_pair(pos, c));//insert back into map at new position

								if(!toLoadList_.in(c))					 //check if chunk is already queued to load
								{
									toLoadList_.push(c);				 //queue chunk load
								}
							}
						}
					}
				}			

				camPosOld = camPosNew;
				playerMoved_ = 0;

				if(toLoadList_.guessSize() > 0)
				{
					toLoadList_.Sort([&](const Chunk* a, const Chunk* b)
					{
						//sort list by distance of chunk from camera
						return DistanceCompare(camPosNew, b->GetCenter(), a->GetCenter());
					});

					//trigger loader threads to start again
					for(int i = 0; i < meshStorage_.size(); i++)
					{
						if(meshStorage_[i]->Ready())
						{
							meshStorage_[i]->Unlock();
						}
					}
				}
			}
		}
	});

	t.detach();	

}

ChunkManager::~ChunkManager()
{
	//doesnt work.
	//nothing is disposed of correctly.
	//many memory leaks.
	//fix it!
	
	finished_ = 1;
	while(	 threadCount_ > 0)
		std::this_thread::sleep_for(std::chrono::seconds(1));
}


void ChunkManager::LoadChunks()
{

	while( loadedList_.guessSize() )
	{
		Chunk* c = loadedList_.pop();
		if(c)
		{
			c->FinaliseMesh();

			meshStorage_[c->getMeshStorageID()]->Unlock();
		}
	}
}

void ChunkManager::UpdateLists()
{
	LoadChunks();

}	

const glm::ivec3 ChunkManager::Pos2ChunkPos(const glm::vec3& pos)
{
	const int bitSize = Chunk::chunkBitSize_;

	const int X = (int)pos.x >> bitSize << bitSize;
	const int Y = (int)pos.y >> bitSize << bitSize;
	const int Z = (int)pos.z >> bitSize << bitSize;

	return glm::vec3(X,Y,Z);
}

Chunk* ChunkManager::ChunkAt(int x, int y, int z)
{
	return ChunkAt(glm::vec3(x,y,z));
}
Chunk* ChunkManager::ChunkAt(const glm::vec3& pos)
{
	return chunkMap_.find(Pos2ChunkPos(pos))->second;
}

void ChunkManager::RenderChunks()
{
	for(unsigned int i = 0; i < chunks_.size(); i++)
	{
		chunks_[i]->Render();		
	}
}

const std::vector<ConcurrentMeshData*>& ChunkManager::GetMeshStorage() const
{
	return meshStorage_;
}