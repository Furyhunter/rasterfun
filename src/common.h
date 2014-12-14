#pragma once

#include <SDL.h>
#include <cstdio>

// basic types
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

extern SDL_Window * gWindow;
extern SDL_Renderer * gRenderer;
extern SDL_Texture * gMainTexture;
extern SDL_Surface * gMainSurface;
extern u32 gWidth;
extern u32 gHeight;

extern FILE * debugOut;

extern bool keyTable[8];
void FlushAndRefresh();
void PollEvents();

extern float fov;

#define DBG(a) fprintf(debugOut, a)

#define FORCEINLINE inline __attribute__((always_inline))
// comment for optimization
//#define FORCEINLINE inline