#pragma once

#include "Block.hpp"
#include "Graphics.hpp"

#define CHUNK_SIZE 32

class Chunk
{
public:
	Chunk(const glm::vec3& position, int id);
	Chunk():meshStorage_(0){};
	~Chunk();				

	void init(const glm::vec3& position, int id);

	void Update();
	void Render();
	void Load(ConcurrentMeshData* meshData, int meshStorageID);
	void FinaliseMesh();
	void GenerateMesh(bool visible);
	void GenrateCube(const glm::ivec3& pos, const glm::ivec3& worldPos);

	void GreedyMesh();

	const bool IsVisible(){return visible_;}

	inline const glm::vec3 GetPosition(){return position_;}
	inline void SetPosition(const glm::vec3& pos)
	{
		visible_ = false;
		position_ = pos;
		center_ = glm::vec3(
		position_.x + (CHUNK_SIZE/2),
		position_.y + (CHUNK_SIZE/2),
		position_.z + (CHUNK_SIZE/2)
		);
	}
	inline const glm::vec3 GetCenter() const;
	inline const int getMeshStorageID(){return meshStorageID_;}

	static const int chunkSize_;
	static const int chunkBitSize_;

	int meshID_;

private:

	const char GetBlockType(const int& x,const int& y,const int& z);
	
	const int AddMeshVertex(const glm::vec3& pos,const glm::vec3& normal,const glm::vec2& uvCoords);

	Block blocks_[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	glm::vec3 position_;
	glm::vec3 center_;
	bool visible_;
	ConcurrentMeshData* meshStorage_;
	int meshStorageID_;

	glm::ivec3 v1p_,v2p_,v3p_,v4p_,v5p_,v6p_,v7p_,v8p_; 

	//These are used a lot and never change.
	static const glm::i8vec3 upNormal_, downNormal_, frontNormal_, backNormal_, leftNormal_, rightNormal_;
	static const float d_;
	static const float uvStep_;

};