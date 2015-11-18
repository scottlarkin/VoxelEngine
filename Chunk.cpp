#include "Chunk.hpp"
#include "SimplexNoise1234.hpp"
#include <random>
#include <math.h>				 
#include "MathHelper.hpp"

const int Chunk::chunkSize_ = CHUNK_SIZE;
const int Chunk::chunkBitSize_ = (int)sqrt(CHUNK_SIZE);
const float Chunk::d_ = 0.02f;
const float Chunk::uvStep_	= 0.16666666666666666666666666666667f;
const glm::i8vec3 Chunk::upNormal_(0,-1,0);
const glm::i8vec3 Chunk::downNormal_(0,1,0);
const glm::i8vec3 Chunk::leftNormal_(-1,0,0);
const glm::i8vec3 Chunk::rightNormal_(1,0,0);
const glm::i8vec3 Chunk::frontNormal_(0,0,-1);
const glm::i8vec3 Chunk::backNormal_(0,0,1);


Chunk::Chunk(const glm::vec3& position, int id):meshStorage_(0)
	,v1p_(0,0,0), v2p_(1,0,0), v3p_(1,0,1), v4p_(0,0,1), v5p_(0,1,0), v6p_(1,1,1), v7p_(0,1,1), v8p_(1,1,0)
{
	init(position, id);								
}

void Chunk::init(const glm::vec3& position, int id)
{
	meshID_ = id;
	SetPosition(position);	
}

void Chunk::Load(ConcurrentMeshData* meshData, int meshStorageID)
{
	meshStorage_ = meshData;
	meshStorageID_ = meshStorageID;
	glm::ivec3 worldPos;
	glm::ivec3 pos;
	Block* b;
	bool visible = false;

	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for(int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{			
				b = &blocks_[i][j][k];
				pos = glm::vec3(i,j,k);
				worldPos = pos + (glm::ivec3)position_;

				if(GetBlockType(worldPos.x, worldPos.y, worldPos.z) == 1)
				{
					b->SetVisible(1);
					visible = true;
				}
				else
				{
					b->SetVisible(0);
				}	
				b->SetVisible(1);
				visible = true;
			}
		}
	}

	GenerateMesh(visible);

}

//used in greedy meshing
class Quad
{
public:
	Quad(){}
	~Quad(){}

	bool operator = (Quad& rhs)
	{
		if(x != rhs.x) return 0;
		if(y != rhs.y) return 0;
		if(w != rhs.w) return 0;
		return h == rhs.h;
	}
	bool CompareSize(Quad& other)
	{
		return h == other.h && w == other.w;
	}

	int x, y, w, h;

};


/*
  This is the function to replace "GenerateMesh()". Work in progress... probably wont get this done
*/
void Chunk::GreedyMesh()
{

	bool sideVerts[chunkSize_][chunkSize_];

	std::vector<vertex> vertices();

	int x1, z1, x2, z2;

	int cubesMeshed = 0;

	for(int y = 0; y < chunkSize_; y++ )
	{

//		if(cubesMeshed == chunkSize_*chunkSize_) 
//			return;


		//compute mask
		for(int i = 0; i < chunkSize_; i++)
		{
			for(int j = 0; j < chunkSize_; j++)
			{
				if(blocks_[i][y][j].IsVisible())
				{
					sideVerts[i][j] = 1;
					cubesMeshed++;
				}
			}
		}

		//generate mesh
		for(int j = 0; j < chunkSize_; j++ )
		{
			for(int i = 0; i < chunkSize_;)
			{
				if(sideVerts[i][j])
				{
					int w, h;

					// Compute width
					for (w = 1; i + w < chunkSize_ && sideVerts[i][w]; w++);


				
				}
				else
				{

				}
			}

		}

		//zero mask
		for(int i = 0; i < chunkSize_; i++)
		{
			for(int j = 0; j < chunkSize_; j++)
			{
				sideVerts[i][j] = 0;
			}
		}

	}

	x1 = 0;
	z1 = 0;
	x2 = chunkSize_;
	z2 = chunkSize_;

	glm::vec3 p1 = glm::vec3(x1,0,z1) ;
	glm::vec3 p2 = glm::vec3(x2,0,z1) ;
	glm::vec3 p3 = glm::vec3(x1,0,z2) ;
	glm::vec3 p4 = glm::vec3(x2,0,z2) ;
	
	int v1 = AddMeshVertex(p1, upNormal_, glm::vec2(0,0));
	int v2 = AddMeshVertex(p2, upNormal_, glm::vec2(0,0));
	int v3 = AddMeshVertex(p3, upNormal_, glm::vec2(0,0));
	int v4 = AddMeshVertex(p4, upNormal_, glm::vec2(0,0));

	meshStorage_->GetData().AddTriangleToMesh(v1, v2, v4);
	meshStorage_->GetData().AddTriangleToMesh(v1, v4, v3);


	visible_ = true;


}


/* 
  Generate the mesh for a chunk...
  This function calls GenerateCube() many times,
  which is a major bottleneck in the application
*/
void Chunk::GenerateMesh(bool visible)
{
	if(!visible)
		return;

	//GreedyMesh();
	//return;

	glm::ivec3 worldPos;
	glm::ivec3 pos;

	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for(int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				if(blocks_[i][j][k].IsVisible())
				{
					pos = glm::vec3(i,j,k);
					worldPos = pos + (glm::ivec3)position_;
					GenrateCube(pos,worldPos);
				}
			}
		}
	}

	visible_ = true;

}

Chunk::~Chunk()
{

}

void Chunk::Render()
{
	if(visible_)
	{
		if(GFX->GetCamera()->GetFrustum()->SphereInFrustum(GetCenter(), chunkSize_/1.5f))
		{
			GFX->RenderMesh(meshID_, position_ );
		}
	}

}
void Chunk::FinaliseMesh()
{

	GFX->GenerateMesh(meshID_, meshStorage_->GetData());

	meshStorage_->GetData().Release();

}

/*
  This function is very slow.
  Look into greedy meshing algorithms as an alternative
*/
void Chunk::GenrateCube(const glm::ivec3& pos, const glm::ivec3& worldPos)
{

	visible_ = true;

	char drawSides;
	bool drawTop = true;
	bool drawBottom = true;
	bool drawLeft = true;
	bool drawRight = true;
	bool drawFront = true;
	bool drawBack = true;
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;	

	//cube vertices.
	glm::ivec3 v1p = v1p_ + pos;
	glm::ivec3 v2p = v2p_ + pos;
	glm::ivec3 v3p = v3p_ + pos;
	glm::ivec3 v4p = v4p_ + pos;
	glm::ivec3 v5p = v5p_ + pos;
	glm::ivec3 v6p = v6p_ + pos;
	glm::ivec3 v7p = v7p_ + pos;
	glm::ivec3 v8p = v8p_ + pos;

	glm::vec2 uvCoords(0,0); 
	int v1, v2, v3, v4, v5, v6, v7, v8; //vertex indexes 

	//below if block filters out any block sides which cant be visible. Reducing vertex count, and preventing graphical artifacts
	if(y > 0)
		drawTop = !blocks_[x][y-1][z].IsVisible();
	if(y < chunkSize_-1)
		drawBottom = !blocks_[x][y+1][z].IsVisible();
	if(x > 0)
	   drawLeft = !blocks_[x-1][y][z].IsVisible();
	if(x < chunkSize_-1)
	   drawRight = !blocks_[x+1][y][z].IsVisible();
	if(z > 0)
	   drawBack = !blocks_[x][y][z-1].IsVisible();
	if(z < chunkSize_-1)
	   drawFront = !blocks_[x][y][z+1].IsVisible();

	////this if block checks the squares next to the outer edge blocks in the chunk to see if they are invisible. If they are
	////visible, dont draw these edges.
	if(y == 0)
		drawTop = !GetBlockType(worldPos.x, worldPos.y - 1, worldPos.z );
	if(y == CHUNK_SIZE-1)
		drawBottom = !GetBlockType(worldPos.x, worldPos.y + 1, worldPos.z );
	if(x == 0)
		drawLeft = !GetBlockType(worldPos.x - 1, worldPos.y, worldPos.z );
	if(x == CHUNK_SIZE-1)
		drawRight = !GetBlockType(worldPos.x + 1, worldPos.y, worldPos.z );
	if(z == 0)
		drawBack = !GetBlockType(worldPos.x , worldPos.y, worldPos.z - 1);
	if(z == CHUNK_SIZE-1)
		drawFront = !GetBlockType(worldPos.x , worldPos.y, worldPos.z + 1);

	////bottom
	if(drawBottom)
	{	
		//uvCoords = glm::vec2(1-uvStep_ + d_,0);
		v5 = AddMeshVertex(v5p, downNormal_, uvCoords);
		//uvCoords = glm::vec2(1 - d_,0);
		v6 = AddMeshVertex(v6p, downNormal_, uvCoords);
		//uvCoords = glm::vec2(1 ,uvStep_ - d_);
		v7 = AddMeshVertex(v7p, downNormal_, uvCoords);
		//uvCoords = glm::vec2(1-uvStep_ + d_,uvStep_ - d_);
		v8 = AddMeshVertex(v8p, downNormal_, uvCoords);

		//tessilate face
		meshStorage_->GetData().AddTriangleToMesh(v8, v5, v7);
		meshStorage_->GetData().AddTriangleToMesh(v8, v7, v6);
	}
	////top
	if(drawTop)
	{
		//uvCoords = glm::vec2(0+d_,0+d_);
		v1 = AddMeshVertex(v1p, upNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_ -d_,0+d_);
		v2 = AddMeshVertex(v2p, upNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_ -d_,uvStep_-d_);
		v3 = AddMeshVertex(v3p, upNormal_, uvCoords);
		//uvCoords = glm::vec2(0+d_,uvStep_ - d_);
		v4 = AddMeshVertex(v4p, upNormal_, uvCoords);

		meshStorage_->GetData().AddTriangleToMesh(v2, v4, v1);
		meshStorage_->GetData().AddTriangleToMesh(v3, v4, v2);
	}

	//left face
	if(drawLeft)
	{
		//uvCoords = glm::vec2(uvStep_,0);
		v1 = AddMeshVertex(v1p, leftNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2, 0);
		v4 = AddMeshVertex(v4p, leftNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2,uvStep_);
		v5 = AddMeshVertex(v5p, leftNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_,uvStep_);
		v7 = AddMeshVertex(v7p, leftNormal_, uvCoords);

		meshStorage_->GetData().AddTriangleToMesh(v4, v5, v1);
		meshStorage_->GetData().AddTriangleToMesh(v4, v7, v5);

	}
	//right face
	if(drawRight)
	{
		//uvCoords = glm::vec2(uvStep_,0);
		v2 = AddMeshVertex(v2p, rightNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2, 0);
		v3 = AddMeshVertex(v3p, rightNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2,uvStep_);
		v6 = AddMeshVertex(v6p, rightNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_,uvStep_);
		v8 = AddMeshVertex(v8p, rightNormal_, uvCoords);

		meshStorage_->GetData().AddTriangleToMesh(v6, v3, v2);
		meshStorage_->GetData().AddTriangleToMesh(v6, v2, v8);
	}
	//front face
	if(drawFront)
	{
		//uvCoords = glm::vec2(uvStep_,0);
		v7 = AddMeshVertex(v7p, frontNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2, 0);
		v6 = AddMeshVertex(v6p, frontNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2,uvStep_);
		v3 = AddMeshVertex(v3p, frontNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_,uvStep_);
		v4 = AddMeshVertex(v4p, frontNormal_, uvCoords);

		meshStorage_->GetData().AddTriangleToMesh(v7, v4, v3);
		meshStorage_->GetData().AddTriangleToMesh(v7, v3, v6);
	}
	//back face
	if(drawBack)
	{
		//uvCoords = glm::vec2(uvStep_,0);
		v1 = AddMeshVertex(v1p, backNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2, 0);
		v2 = AddMeshVertex(v2p, backNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_*2,uvStep_);
		v5 = AddMeshVertex(v5p, backNormal_, uvCoords);
		//uvCoords = glm::vec2(uvStep_,uvStep_);
		v8 = AddMeshVertex(v8p, backNormal_, uvCoords);

		meshStorage_->GetData().AddTriangleToMesh(v5, v2, v1);
		meshStorage_->GetData().AddTriangleToMesh(v5, v8, v2);
	}
}

const int Chunk::AddMeshVertex(const glm::vec3& pos,const glm::vec3& normal,const glm::vec2& uvCoords)
{
	vertex v;
	v.pos = pos;
	v.normal = normal;
	v.texCoords = uvCoords;

	meshStorage_->GetData().AddVertexToMesh(v);

	return meshStorage_->GetData().GetVertexCount() - 1;
}

const char Chunk::GetBlockType(const int& x,const int& y,const int& z)
{	   


	//uses 3d simplex noise to get the type of block at a given world coordinate.
	/*if(y <= 0)
	return 0;

	if(SimplexNoise1234().noise(
	(float)x*0.03f ,
	(float)y*0.03f ,
	(float)z*0.03f ) * (float)y * 0.001f
	> 0)
	return 0;*/

	float n = (SimplexNoise1234().noise((float)x*0.005f , (float)z*0.004f) * 0.5) +0.5;


	//float c =  SimplexNoise1234().noise(
	//				(float)x*0.03f ,
	//				(float)y*0.03f ,
	//				(float)z*0.03f ) ;//* n;

	//float s = -((c*0.5+0.5)*35.f); //create surface entrances to the caverns

	n*= -75; //75 is highest point above water level

	if(y >= n)
	{
		return 1 ;//- ((y > s) && (c >= 0));
	}

	return 0;

}

const glm::vec3 Chunk::GetCenter() const
{
	return center_;
}