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

FILE * debugOut;


bool gRunning = true;

static Vector4 pixel_shader()
{
	using namespace shader_in;

	return Vector4(screen_pos.x, screen_pos.y, 1-screen_pos.x, 1);
}

int main(int argc, char ** argv)
{
	SDL_Event evt;
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

	SetShader(pixel_shader);
	while (gRunning)
	{
		while (SDL_PollEvent(&evt))
		{
			if (evt.type == SDL_QUIT)
			{
				gRunning = false;
			}
			if (evt.type = SDL_KEYDOWN)
			{
				if (evt.key.keysym.sym == SDLK_ESCAPE)
				{
					gRunning = false;
				}
				if (evt.key.keysym.sym == SDLK_LEFT)
				{
					rot += 5;
				}
				if (evt.key.keysym.sym == SDLK_RIGHT)
				{
					rot -= 5;
				}
			}
	
		}

		// draw
		std::cout << "drawing frame" << std::endl;
		BeginDraw();
		memset(gMainSurface->pixels, 0, gMainSurface->w * gMainSurface->h * 4);
		shader_in::view.identity().rotate(rot, 0, 1, 0);
		shader_in::model.identity();
		//shader_in::model.rotate(timeRun * 90, 1, 0, 0);
		shader_in::model.translate(0, 0, timeRun);
		//shader_in::model.translate(timeRun, 0, 0);
		/*DrawTriangle(
			Vector3(0, 100, -50 - timeRun * 64),
			Vector3(100, 200, 0),
			Vector3(0, 200, 0));*/
		DrawTriangle(
			Vector4(0, 0, 0, 1),
			Vector4(0, 1, 0, 1),
			Vector4(1, 0, 0, 1));

		// floor
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