#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstring>

#include "common.h"

#include "primdraw.h"
#include "matrix.h"

SDL_Surface * gMainSurface;
SDL_Texture * gMainTexture;

SDL_Renderer * gRenderer;
SDL_Window * gWindow;

u32 gWidth;
u32 gHeight;

float fov = 45;

FILE * debugOut;

bool keyTable[5] = {false, false, false, false, false};

bool gRunning = true;

static Vector4 pixel_shader()
{
	using namespace shader_in;

	return Vector4(screen_pos.x, screen_pos.y, 1-screen_pos.x, 1);
}

static Vector4 psh_wall()
{
	using namespace shader_in;

	return Vector4(.05f, .1f, 0, 1);
}

void FlushAndRefresh()
{
	SDL_UpdateTexture(gMainTexture, NULL, gMainSurface->pixels, gMainSurface->pitch);
	SDL_RenderCopy(gRenderer, gMainTexture, NULL, NULL);
	SDL_RenderCopyEx(gRenderer, gMainTexture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderPresent(gRenderer);
}

void PollEvents()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		if (evt.type == SDL_QUIT)
		{
			gRunning = false;
		}
		if (evt.type == SDL_KEYDOWN)
		{
			if (evt.key.keysym.sym == SDLK_ESCAPE)
			{
				gRunning = false;
			}
			switch (evt.key.keysym.sym)
			{
				case SDLK_LEFT:  keyTable[0] = true; break;
				case SDLK_RIGHT: keyTable[1] = true; break;
				case SDLK_UP:    keyTable[2] = true; break;
				case SDLK_DOWN:  keyTable[3] = true; break;
				case SDLK_q:     keyTable[4] = true; break;
				case SDLK_e:     keyTable[5] = true; break;
			}
		}
		if (evt.type == SDL_KEYUP)
		{
			switch (evt.key.keysym.sym)
			{
				case SDLK_LEFT:  keyTable[0] = false; break;
				case SDLK_RIGHT: keyTable[1] = false; break;
				case SDLK_UP:    keyTable[2] = false; break;
				case SDLK_DOWN:  keyTable[3] = false; break;
				case SDLK_q:     keyTable[4] = false; break;
				case SDLK_e:     keyTable[5] = false; break;
			}
		}
	}
}

int main(int argc, char ** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	debugOut = fopen("debug.txt", "w");

	gWindow = SDL_CreateWindow("butts", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	gWidth =  320;
	gHeight = 240;
	gRenderer = SDL_CreateRenderer(gWindow, 0, 0);

	gMainSurface = SDL_CreateRGBSurface(0, gWidth, gHeight, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	gMainTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, gWidth, gHeight);

	SDL_Rect renderRect;
	float timeRun = 0;
	float rot = 0;
	float zpos = 0;

	while (gRunning)
	{
		PollEvents();

		if (keyTable[0]) rot+=90 * (1.f/60);
		if (keyTable[1]) rot-=90 * (1.f/60);
		if (keyTable[2]) zpos-=10 * (1.f/60);
		if (keyTable[3]) zpos+=10 * (1.f/60);
		if (keyTable[4]) fov+=20*(1.f/60);
		if (keyTable[5]) fov-=20*(1.f/60);

		// draw
		BeginDraw();
		memset(gMainSurface->pixels, 0, gMainSurface->w * gMainSurface->h * 4);
		shader_in::view.identity().translate(0, 0, zpos).rotate(-rot, 0, 1, 0);
		shader_in::model.identity();
		//shader_in::model.rotate(timeRun * 90, 1, 0, 0);
		//shader_in::model.translate(0, 0, timeRun);
		//shader_in::model.translate(timeRun, 0, 0);
		/*DrawTriangle(
			Vector3(0, 100, -50 - timeRun * 64),
			Vector3(100, 200, 0),
			Vector3(0, 200, 0));*/
		/*DrawTriangle(
			Vector4(0, 0, 0, 1),
			Vector4(0, 1, 0, 1),
			Vector4(1, 0, 0, 1));*/

		// wall
		SetShader(psh_wall);
		shader_in::model.identity().scale(10).translate(-5, -3, 13);
		DrawTriangle(
			Vector4(0, 0, 0, 1),
			Vector4(0, .5f, 0, 1),
			Vector4(1, 0, 0, 1));
		DrawTriangle(
			Vector4(1, 0, 0, 1),
			Vector4(0, .5f, 0, 1),
			Vector4(1, .5f, 0, 1));

		// floor
		SetShader(pixel_shader);
		shader_in::model.identity().scale(10).translate(-5, 1.8, 3);
		//shader_in::model.identity().rotate(90, -1,0,0).scale(10).translate(0, -1, 0);
		DrawTriangle(
			Vector4(0, 0, 0, 1),
			Vector4(0, 0, 1, 1),
			Vector4(1, 0, 0, 1));
		DrawTriangle(
			Vector4(1, 0, 0, 1),
			Vector4(0, 0, 1, 1),
			Vector4(1, 0, 1, 1));

		EndDraw();

		// finish blit
		SDL_UpdateTexture(gMainTexture, NULL, gMainSurface->pixels, gMainSurface->pitch);
		SDL_RenderCopy(gRenderer, gMainTexture, NULL, NULL);
		SDL_RenderCopyEx(gRenderer, gMainTexture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(gRenderer);


		SDL_Delay(1000/60);
		timeRun += (1.f/60.f);
	}

	//SDL_FreeSurface(gMainSurface);
	//SDL_DestroyTexture(gMainTexture);
	//SDL_DestroyRenderer(gRenderer);
	//SDL_DestroyWindow(gWindow);
	SDL_Quit();
	fclose(debugOut);
	return 0;
}