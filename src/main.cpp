#include <cstdio>
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


bool gRunning = true;

static Vector4 pixel_shader()
{
	using namespace shader_in;

	return Vector4(1, 0, 0, 1);
}

int main(int argc, char ** argv)
{
	SDL_Event evt;
	SDL_Init(SDL_INIT_VIDEO);

	gWindow = SDL_CreateWindow("butts", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	gWidth = 160;
	gHeight = 120;
	gRenderer = SDL_CreateRenderer(gWindow, 0, 0);

	gMainSurface = SDL_CreateRGBSurface(0, gWidth, gHeight, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	gMainTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, gWidth, gHeight);

	SDL_Rect renderRect;
	float timeRun = 0;

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
			}
	
		}

		// draw
		BeginDraw();
		memset(gMainSurface->pixels, 0, gMainSurface->w * gMainSurface->h * 4);
		//model.rotate(timeRun * 90, 1, 0, 0);
		//shader_in::model.translate(0, 0, timeRun * 10);
		/*DrawTriangle(
			Vector3(0, 100, -50 - timeRun * 64),
			Vector3(100, 200, 0),
			Vector3(0, 200, 0));*/
		DrawTriangle(
			Vector4(0, 0, 0, 1),
			Vector4(1, 0, 10, 1),
			Vector4(0, 1, 10, 1));
		EndDraw();

		// finish blit
		SDL_UpdateTexture(gMainTexture, NULL, gMainSurface->pixels, gMainSurface->pitch);
		SDL_RenderCopy(gRenderer, gMainTexture, NULL, NULL);
		SDL_RenderPresent(gRenderer);


		SDL_Delay(1000/60);
		timeRun += (1.f/60.f);
	}

	//SDL_FreeSurface(gMainSurface);
	//SDL_DestroyTexture(gMainTexture);
	//SDL_DestroyRenderer(gRenderer);
	//SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}