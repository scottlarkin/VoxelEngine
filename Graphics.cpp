#include "Graphics.hpp"
#include <cstddef>
#include "FileHelper.hpp"
#include <iostream>
#include <cassert>
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstring>
#include "MathHelper.hpp"

#define BUFFER_OFFSET_OF(structure,member) ((char*)&(((structure*)0)->member))

Graphics* Graphics::gfx_ = 0;

void Graphics::LoadShader(std::string path, GLuint& shader, GLuint Program)
{
	GLint compile_status = 0;
	std::string file = stringFromFile(path);

	const char* vertexShaderSource = file.c_str(); 

	glShaderSource(shader, 1, (const GLchar **)& vertexShaderSource, NULL );

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) 
	{
		const int string_length = 1024;
		GLchar log[string_length]= "";
		glGetShaderInfoLog(shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
	glAttachShader(Program, shader);

}

void Graphics::SetCamera(Camera* cam)
{
	camera_ = cam;

	projectionMatrix_ = glm::perspective(camera_->vertical_field_of_view_degrees_,
										 aspect_ratio_ ,
										 camera_->near_plane_distance_,
										 camera_->far_plane_distance_);

}

void Graphics::InitGL()
{

	//set up Open GL
	GraphicsShader_  = glCreateProgram();
	TextShader_ = glCreateProgram();

	//load vertex shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	LoadShader("VertexShader.glsl", vertexShader, GraphicsShader_);

	glBindAttribLocation(GraphicsShader_, 0, "vertex_position");
	glBindAttribLocation(GraphicsShader_, 1, "vertex_normal");
	glBindAttribLocation(GraphicsShader_, 2, "texCoords");
	glDeleteShader(vertexShader);

	GLuint text_vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	LoadShader("Text_VertexShader.glsl", text_vertexShader, TextShader_);
	glDeleteShader(text_vertexShader);

	//load fragment shaders									  
	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	LoadShader("FragmentShader.glsl", fragmentShader, GraphicsShader_);
	glBindFragDataLocation(GraphicsShader_, 0, "fragment_colour");
	glDeleteShader(fragmentShader);

	GLuint text_fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	LoadShader("Text_FragmentShader.glsl", text_fragmentShader, TextShader_);
	glBindFragDataLocation(TextShader_, 0, "fragment_colour");
	glDeleteShader(text_fragmentShader);

	//Link programs
	GLint link_status = 0;
	glLinkProgram( GraphicsShader_ );
	glGetProgramiv(GraphicsShader_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length]= "";
		glGetProgramInfoLog(GraphicsShader_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	glLinkProgram( TextShader_ );
	glGetProgramiv(TextShader_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length]= "";
		glGetProgramInfoLog(TextShader_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	//set clear colour
	glClearColor( 0.16f, 0.525f,0.9f, 1.f );  

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	aspect_ratio_ = viewport_size[2] / (float)viewport_size[3];

	upDir_ = glm::vec3(0,1,0);

	glEnable(GL_TEXTURE_2D);
	glUseProgram( GraphicsShader_ );
	glEnable(GL_CULL_FACE);	 
	glEnable(GL_DEPTH_TEST); 
	glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );		   //RENDER LINES INSTEAD OF FACES

	textures_ = LoadTextue( "Images/Textures.png");
	font_ =  LoadTextue( "Images/font.bmp");
}

void Graphics::InitSDL()
{
	gameWindow_= 0;
 
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	//Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	gameWindow_ = SDL_CreateWindow("MineWest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth_, screenHeight_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	context_ = SDL_GL_CreateContext(gameWindow_);

	std::string p = "OpenGL context could not be created! SDL Error: %s\n   ";
	p +=  std::string(SDL_GetError());

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();


	//use v-sync
	//SDL_GL_SetSwapInterval(1);

}

Graphics::Graphics(int w, int h)
{
	screenWidth_ = w;
	screenHeight_ = h;

	InitSDL();
	InitGL();
	
}

Graphics* Graphics::GetInstance()
{
	if(!gfx_)
		return 0;

	return gfx_;
}

Graphics::~Graphics()
{
	delete gfx_;
}

bool Graphics::Init(int w, int h)
{
	if(!gfx_)
		gfx_ = new Graphics(w, h);

	return 1;
}

void Graphics::Close()
{

	SDL_DestroyWindow(gameWindow_);
	gameWindow_ = 0;

	SDL_Quit();

	delete gfx_;

}
   
void Graphics::PreRender()
{
	
	//TODO:
		//Use render buffer objects instead of uniform variables...
	
	/*
	  This code will only work for 1 light source,
	  Convert to multi-pass rendering, 1 pass per light.
	  Possible deferred renderer, but will need to write code to generate light source geometry, as well as a few shaders
	*/
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //IMPORTANT
	
	//set up the view and projection matrices
	camLookAt_ = camera_->GetPosition() + camera_->Direction();
	
	//2 lines below only need to be executed if the camera moved since last render call!
	view_xform_ = glm::lookAt(camera_->GetPosition(), camLookAt_, upDir_);

	//probably be beter to do this computation on the GPU as the CPU appears to the the bottleneck at the moment
	projectionView_ = projectionMatrix_ * view_xform_; 
	
	//this can be done once on itit, not once per frame!
	glm::vec3 skyColour(0.16f, 0.525f,0.9f);

	GLuint camPos = glGetUniformLocation(GraphicsShader_, "camera_position");
	glUniform3f(camPos, camera_->GetPosition().x,  camera_->GetPosition().y,  camera_->GetPosition().z);
	
	//can be done once... should NOT be done per frame.
	GLuint skyCol = glGetUniformLocation(GraphicsShader_, "skyColour");
	glUniform3f(skyCol, skyColour.x,  skyColour.y,  skyColour.z);
	

	//per light source. onkly 1 at the minute but this will change. 
	GLuint loc = glGetUniformLocation(GraphicsShader_, "light_source.position");
	glUniform3f(loc, camera_->GetPosition().x,  camera_->GetPosition().y,  camera_->GetPosition().z);

	GLuint dir = glGetUniformLocation(GraphicsShader_, "light_source.direction");
	glUniform3f(dir, camera_->Direction().x,  camera_->Direction().y,  camera_->Direction().z);

	GLuint fov = glGetUniformLocation(GraphicsShader_, "light_source.field_of_view");
	glUniform1f(fov, 5);

	GLuint rng = glGetUniformLocation(GraphicsShader_, "light_source.range");
	glUniform1f(rng, 1000);

	GLuint projectionView_id = glGetUniformLocation(GraphicsShader_, "projection_view_xform");
	glUniformMatrix4fv(projectionView_id , 1 , GL_FALSE, glm::value_ptr(projectionView_ ));

}

void Graphics::SwapBuffer()
{
	SDL_GL_SwapWindow(gameWindow_);
}

const GLuint Graphics::LoadTextue(char* filepath)
{
	TextureProperties properties(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, 16);

	GLint numColours = 0;
	GLenum format;
	SDL_Surface* img = IMG_Load(filepath);
	GLuint id;

	if(img->format->BytesPerPixel == 4)
	{
		if(img->format->Rmask == 0x000000ff)
		{
			format = GL_RGBA;
		}
		else
		{
			format = GL_BGRA;
		}
		numColours = 4;
	}
	else
	{
		if(img->format->Rmask == 0x000000ff)
		{
			format = GL_RGB;
		}
		else
		{
			format = GL_BGR;
		}
		numColours = 3;	
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)properties.GetMagFilter());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)properties.GetMinFilter());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat)properties.GetWrap());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat)properties.GetWrap());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)properties.GetAnisotropy());

	glTexImage2D(GL_TEXTURE_2D, 0, numColours=4?GL_RGBA:GL_RGB, img->w, img->h,0,format, GL_UNSIGNED_BYTE, img->pixels);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(img);

	return id;

}

void Graphics::GenerateMesh(int meshID, const MeshData& mesh)
{
	//TODO:
		//This was written when I had little experience with OpenGL... Improve this
			//allocate buffer memory once per chunk, use bufferSubData to re-allocate memory
			//why am I generating new VAO/ VBO per call..? Fix it. 
			//GL_STATIC_DRAW should probably be GL_DYNAMIC_DRAW, as the data will change 

	meshes_[meshID].element_count = mesh.GetElementCount();

	//create the buffer object 
	glGenBuffers(1, &meshes_[meshID].vertex_data_vbo);
	//bind the buffer object in the mesh
	glBindBuffer(GL_ARRAY_BUFFER, meshes_[meshID].vertex_data_vbo);
	//fill buffer object with data
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVertexCount() * sizeof(vertex), mesh.GetVertexData() , GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//same as above for element vbo
	glGenBuffers(1, &meshes_[meshID].element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_[meshID].element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes_[meshID].element_count * sizeof(unsigned int), mesh.GetElementData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//generate the vertex array object
	glGenVertexArrays(1, &meshes_[meshID].vao);

	//bind vertex array
	glBindVertexArray(meshes_[meshID].vao);
	//bind the element and vertex buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_[meshID].element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, meshes_[meshID].vertex_data_vbo);

	//put normal data in the 1st position of the vertex array
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET_OF(vertex, pos));

	//put position data in 2nd position of the array
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET_OF(vertex, normal));

	//texture data goes in 3rd position
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET_OF(vertex, texCoords));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
}

int Graphics::AllocateMesh()
{
	Mesh m;
	meshes_.push_back(m);
	return meshes_.size() - 1;
}

void Graphics::RenderMesh(int meshID, const glm::vec3& pos)
{

	//send uniform vars to GPU
	glm::mat4 modelXform = translate(pos.x, pos.y, pos.z)  ;
	GLuint model_xform_id = glGetUniformLocation(GraphicsShader_, "model_xform");
	glUniformMatrix4fv(model_xform_id ,1, GL_FALSE, glm::value_ptr(modelXform));
																																		
	//texture the mesh
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures_);
	GLuint sampler_id = glGetUniformLocation(GraphicsShader_, "sampler");
	glUniform1i(sampler_id, 0);

	//bind mesh vertix array
	glBindVertexArray(meshes_[meshID].vao);

	//render mesh... 
	//at the moment there is 1 draw call per mesh, instanced drawing wont work because every mesh is different
	//look into glMultiDrawElements
	glDrawElements(GL_TRIANGLES,meshes_[meshID].element_count,GL_UNSIGNED_INT,0);
		

	//Lesson learned... Unbind shit when your done with it! 
	glBindVertexArray(0);

}

void Graphics::DrawLine(float width, glm::vec3 colour,  glm::vec3 start,  glm::vec3 end )
{

	//dont think this works properly.

	glm::vec3 p1 ;//= glm::vec3(view_xform_ * glm::vec4(start, 1));
	glm::vec3 p2 ;//= glm::vec3(view_xform_ * glm::vec4(end, 1));
	p1 = start;
	p2 = end;
	glLineWidth(width); 
	glColor3f(colour.x,colour.y, colour.z);
	glBegin(GL_LINES);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();

}

void loadText()
{
	//TODO

}

void Graphics::RenderText(GLint x, GLint y, char* text)
{

	//this doesnt work at all. 
	//I now know better... write new glsl shaders for text

	glUseProgram(0);
	int length = strlen(text);
	glColor4f(1.0,1.0,1.0,1.0);


	glBegin(GL_QUADS);
	glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner  
	glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner 
	glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner  
	glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner  
	glEnd();


}