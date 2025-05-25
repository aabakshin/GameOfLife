#ifndef SDL_EXT_HPP
#define SDL_EXT_HPP

#include "cleanup.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

void logSDLError(std::ostream& out, const char* err_msg);
SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* ren);
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, int w, int h);
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, SDL_Rect dst, SDL_Rect* clip = nullptr);
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, SDL_Rect* clip = nullptr);
SDL_Texture* renderText(const std::string& message, const std::string& fontFile,
		SDL_Color color, int fontSize, SDL_Renderer* renderer);

#endif
