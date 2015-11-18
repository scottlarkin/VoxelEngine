#include "Mesh.hpp"
#include <iostream>

Mesh::Mesh() : 
	   element_vbo(0),
	   vertex_data_vbo(0),
	   vao(0),
	   element_count(0) 
{
	

}

Mesh::~Mesh(){}

MeshData::MeshData(const int chunkSize): vertexCount_(0), elementCount_(0)
{	
	meshVertex_.resize(pow(chunkSize,3) * 24);
	meshElement_.resize(pow(chunkSize,3) * 36);

}

MeshData::~MeshData()
{

}

void MeshData::Clear()
{
	meshElement_.clear();
	meshVertex_.clear();
	
}

void MeshData::AddVertexToMesh(const vertex& v)  
{
	meshVertex_[vertexCount_] = v;
	vertexCount_++;
}

void MeshData::AddTriangleToMesh(const int& v1, const int& v2, const int& v3) 
{
	meshElement_[elementCount_] = v1;
	meshElement_[elementCount_+1] = v2;
	meshElement_[elementCount_+2] = v3;
	elementCount_+= 3;
}

void MeshData::Release()
{
	elementCount_ = 0;
	vertexCount_ = 0;
}

void ConcurrentMeshData::Lock()
{
	inUse_ = 1;
	ready_ = 0;
}

void ConcurrentMeshData::Unlock()
{
	inUse_ = 0;
	NotifyCondition();
}

bool ConcurrentMeshData::IsInUse()
{
	return inUse_;
}

MeshData& ConcurrentMeshData::GetData()
{
	return data_;
}

void ConcurrentMeshData::Wait(std::unique_lock<std::mutex>& lock)
{
	condition_.wait(lock, [=]{ 
	   return !inUse_;
	});
}

void ConcurrentMeshData::NotifyCondition()
{
	condition_.notify_all();
}

std::mutex& ConcurrentMeshData::GetMutex()
{
	return lock_;
}

void ConcurrentMeshData::SetReady(bool r)
{
	ready_ = r;
}

bool ConcurrentMeshData::Ready()
{
	return ready_;
}