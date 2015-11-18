#pragma once
#include <vector>	 
#include <SDL_ttf.h>
#include "glew.h"
#include "SDL_opengl.h"
#include "glm\glm.hpp"
#include <iostream>
#include "Mesh.hpp"
#include "Camera.hpp"
#include "ThreadSafeContainers.hpp"



#define GFX Graphics::GetInstance()


class Graphics
{
	public:

		float GetScreenRatio(){return aspect_ratio_;}
		void SetCamera(Camera* cam);
		const Camera* GetCamera(){return camera_;}

		static bool Init(int w, int h);
		static Graphics* GetInstance();
		
		void Close();

		void PreRender();
		void SwapBuffer();

		void RenderText(GLint x, GLint y, char* text);
		void DrawLine(float width, glm::vec3 colour,  glm::vec3 start,  glm::vec3 end );

		int AllocateMesh();
		void GenerateMesh(int meshID,const MeshData& mesh);
		void RenderMesh(int meshID, const glm::vec3& pos);

		const GLuint LoadTextue(char* filepath);
		GLuint textures_;
		GLuint font_;

	private:

		//pointer to graphics singleton
		static Graphics* gfx_;

		~Graphics();
		//init functions
		Graphics(int w, int h);
		void InitGL();
		void InitSDL();
		
		void LoadShader(std::string path, GLuint& shader, GLuint Program);
		
		int screenWidth_;
		int screenHeight_;
		float aspect_ratio_;

		Camera* camera_;

		glm::vec3 camLookAt_;
		glm::mat4 view_xform_;
		//OpenGL Vars
		SDL_Window* gameWindow_;
		SDL_GLContext context_;
		GLuint GraphicsShader_;
		GLuint TextShader_;
		
	    glm::vec3 upDir_;
		glm::mat4 projectionMatrix_;
		glm::mat4 projectionView_;

		std::vector<Mesh> meshes_;

};


