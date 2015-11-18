#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Graphics.hpp"

class Text
{
	Text() ;
	~Text()	;

	void LoadText(std::string path, std::string textureText, SDL_Color textColor);
	void RenderText();
	
	void Clean();
	
	SDL_Texture* textTexture_;
	std::string text_;
	SDL_Color colour_;

};
