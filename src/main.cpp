#include <cstdio>

#include "common.h"

#include "primdraw.h"
#include "matrix.h"

SDL_Surface * gMainSurface;
SDL_Texture * gMainTexture;

SDL_Renderer * gRenderer;
SDL_Window * gWindow;

u32 gWidth;
u32 gHeight;

bool gRunning = true;

int main(int argc, char ** argv)
{
	SDL_Event evt;
	SDL_Init(SDL_INIT_VIDEO);

	gWindow = SDL_CreateWindow("butts", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	gWidth = 640;
	gHeight = 480;
	gRenderer = SDL_CreateRenderer(gWindow, 0, 0);

	gMainSurface = SDL_CreateRGBSurface(0, gWidth, gHeight, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	gMainTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, gWidth, gHeight);

	SDL_Rect renderRect;
	float timeRun = 0;

	SetShader([]() { return Vector4((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1); });
	while (gRunning)
	{
		while (SDL_PollEvent(&evt))
		{
			if (evt.type == SDL_QUIT)
			{
				gRunning = false;
				break;
			}
	
		}

		// draw
		BeginDraw();
		memset(gMainSurface->pixels, 0, gMainSurface->w * gMainSurface->h * 4);
		DrawTriangle(
			Vector3(150 + timeRun * 10, 100, 0),
			Vector3(200, 200, 0),
			Vector3(100, 200, 0));
		EndDraw();

		// finish blit
		SDL_UpdateTexture(gMainTexture, NULL, gMainSurface->pixels, gMainSurface->pitch);
		SDL_RenderCopy(gRenderer, gMainTexture, NULL, NULL);
		SDL_RenderPresent(gRenderer);


		SDL_Delay(1000/60);
		timeRun += (1.f/60.f);
	}


	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}