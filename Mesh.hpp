#pragma once
#include "glm\glm.hpp"
#include "SDL_opengl.h"
#include <vector>
#include <mutex>
#include <condition_variable>

struct vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
};


//stores the openGL buffers
class Mesh
{
public:
	Mesh();
	~Mesh();

	GLuint element_vbo;
	GLuint vertex_data_vbo;
	GLuint vao;
	int element_count;
};


class MeshData
{
public:
	MeshData(const int chunkSize);
	~MeshData();
	void Clear();
	void AddVertexToMesh(const vertex& v) ;
	void AddTriangleToMesh(const int& v1, const int& v2, const int& v3) ;
	const int GetVertexCount() const {return vertexCount_;}//meshVertex_.size();}
	const int GetElementCount() const {return elementCount_;}//meshElement_.size();}
	const void* GetVertexData() const {return meshVertex_.data(); }
	const void* GetElementData() const {return meshElement_.data(); }

	void Release();

private:
	int vertexCount_;
	int elementCount_;
	std::vector<vertex> meshVertex_;
	std::vector<int> meshElement_;
};

class ConcurrentMeshData
{
public:
	ConcurrentMeshData(int id, const int chunkSize):data_(chunkSize), id_(id), inUse_(0),ready_(1){}
	~ConcurrentMeshData(){}

	bool IsInUse();
	void Lock();
	void Unlock();		
	void SetReady(bool r);
	void NotifyCondition();
	MeshData& GetData();
	std::mutex& GetMutex();
	void Wait(std::unique_lock<std::mutex>& lock);
	bool Ready();
private:
	
	MeshData data_;
	int id_;
	bool inUse_;
	bool ready_;
	std::mutex lock_;
	std::condition_variable condition_;


};