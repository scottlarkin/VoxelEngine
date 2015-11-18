#pragma once
#include "SDL.h"
#include <SDL_image.h>

class Image
{
public:

	Image(int w, int h);
	~Image();

	const int GetWidth(){return width_;}
	const int GetHeight(){return height_;}
	SDL_Texture* GetImg(){return img_;}
	
	void SetImage(SDL_Texture* img){img_ = img;}

private:
	int width_;
	int height_;
	SDL_Texture* img_;
	
};