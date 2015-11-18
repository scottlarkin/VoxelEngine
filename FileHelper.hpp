#pragma once
#include <string>
#include <SDL.h>

#include <iostream>
#include <SDL_image.h>

std::string stringFromFile(std::string filepath);

class TextureProperties
{

public:

	TextureProperties(int magFilter, int minFilter, int wrap, int anisotropy );

	int GetMagFilter(){return magFilter_;}
	int GetMinFilter(){return minFilter_;}
	int GetWrap(){return wrap_;}
	int GetAnisotropy(){return anisotropy_;}

private:

	int magFilter_;
	int minFilter_;
	int wrap_;
	int anisotropy_;


};

unsigned int GetTexture(char* filepath, TextureProperties properties);
