#include "Image.hpp"



Image::Image(int w, int h):width_(w),height_(h)
{

}

Image::~Image()
{
	SDL_DestroyTexture(img_);
}