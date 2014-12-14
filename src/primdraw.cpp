#include "primdraw.h"

#include "common.h"

#include "matrix.h"

#include <algorithm>
#include <functional>
#include <cstdio>

using namespace std;

namespace shader_in
{
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 resolution;
	Vector2 screen_pos;
};

using namespace shader_in;

static int triangleCounter = 0;

static function<Vector4()> PixelShader(nullptr);

///////////////////////////////////////////////////////////////////////////////
// glFrustum()
///////////////////////////////////////////////////////////////////////////////
static Matrix4 setFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4 mat;
    mat[0]  = 2 * n / (r - l);
    mat[5]  = 2 * n / (t - b);
    mat[8]  = (r + l) / (r - l);
    mat[9]  = (t + b) / (t - b);
    mat[10] = -(f + n) / (f - n);
    mat[11] = -1;
    mat[14] = -(2 * f * n) / (f - n);
    mat[15] = 0;
    return mat;
}

///////////////////////////////////////////////////////////////////////////////
// gluPerspective()
///////////////////////////////////////////////////////////////////////////////
static Matrix4 setFrustum(float fovY, float aspect, float front, float back)
{
    float tangent = tanf(fovY/2 * DEG2RAD); // tangent of half fovY
    float height = front * tangent;         // half height of near plane
    float width = height * aspect;          // half width of near plane

    // params: left, right, bottom, top, near, far
    return setFrustum(-width, width, -height, height, front, back);
}

///////////////////////////////////////////////////////////////////////////////
// glOrtho()
///////////////////////////////////////////////////////////////////////////////
Matrix4 setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4 mat;
    mat[0]  = 2 / (r - l);
    mat[5]  = 2 / (t - b);
    mat[10] = -2 / (f - n);
    mat[12] = -(r + l) / (r - l);
    mat[13] = -(t + b) / (t - b);
    mat[14] = -(f + n) / (f - n);
    return mat;
}

/*static inline s32 orient2d(Vector3 a, Vector3 b, s32 px, s32 py)
{
	return (b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x);
}*/

void BeginDraw()
{
	triangleCounter = 0;

	model = Matrix4().identity();
	view = Matrix4().identity();
	resolution = Matrix4().identity();
	projection = Matrix4().identity();

	//projection = setOrthoFrustum(-1, 1, -1, 1, 0, 1);
	projection = setFrustum(fov, ((float)gWidth/gHeight), .001f, 10);
	//projection = setFrustum(-1, 1, -1, 1, -1, 1);
	resolution.translate(1, 1, 0);
	resolution.scale(gWidth/2, gHeight/2, 1);

	SDL_LockSurface(gMainSurface);
}

void EndDraw()
{
	SDL_UnlockSurface(gMainSurface);

	fprintf(debugOut, "END DRAW\n\n");
}

void SetShader(function<Vector4()> func)
{
	PixelShader = func;
}

/*void DrawTriangle(Vector4 v1, Vector4 v2, Vector4 v3)
{
	Matrix4 mvp = resolution * projection * view * model; //model * view * projection * resolution;
	v1 = mvp * v1;
	v2 = mvp * v2;
	v3 = mvp * v3;

	Vector3 hv1, hv2, hv3;
	hv1 = Vector3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w);
	hv2 = Vector3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w);
	hv3 = Vector3(v3.x/v3.w, v3.y/v3.w, v3.z/v3.w);
	s32 minX = round(min(hv1.x, min(hv2.x, hv3.x)));
	s32 minY = round(min(hv1.y, min(hv2.y, hv3.y)));
	s32 maxX = round(max(hv1.x, max(hv2.x, hv3.x)));
	s32 maxY = round(max(hv1.y, max(hv2.y, hv3.y)));

	// screen bounds
	minX = max(minX, 0);
	minY = max(minY, 0);
	maxX = min(maxX, (s32)gWidth);
	maxY = min(maxY, (s32)gHeight);

	s32 px, py;
	for (py = minY; py <= maxY; py++)
	{
		for (px = minX; px < maxX; px++)
		{
			s32 w0 = orient2d(hv2, hv3, px, py);
			s32 w1 = orient2d(hv3, hv1, px, py);
			s32 w2 = orient2d(hv1, hv2, px, py);

			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				// prepare shader inputs
				screen_pos = Vector2((float)px / gWidth, (float)py / gHeight);
				Vector4 color;
				if (PixelShader)
					color = PixelShader();
				else
					color = Vector4(1,0,0,1);
				u8 * pixels = (u8*)gMainSurface->pixels;
				pixels[(px + (py * gMainSurface->w))*4 + 0] = round(color.w * 255);
				pixels[(px + (py * gMainSurface->w))*4 + 1] = round(color.z * 255);
				pixels[(px + (py * gMainSurface->w))*4 + 2] = round(color.y * 255);
				pixels[(px + (py * gMainSurface->w))*4 + 3] = round(color.x * 255);
			}
		}
	}
}*/

static FORCEINLINE void _PutPixel(s32 x, s32 y, u8 r, u8 g, u8 b, u8 a)
{
	u8 * pixels = (u8*)gMainSurface->pixels;
	//fprintf(debugOut, "%d %d %d %d", r,g,b,a);
	pixels[(x + (y * gMainSurface->w))*4 + 0] = a;
	pixels[(x + (y * gMainSurface->w))*4 + 1] = b;
	pixels[(x + (y * gMainSurface->w))*4 + 2] = g;
	pixels[(x + (y * gMainSurface->w))*4 + 3] = r;
}

static FORCEINLINE void _DrawSpan(float x1, float x2, s32 y)
{
	if (y < 0)
		return;
	if (y >= gHeight)
		return;

	/*
	float xmin = min(x1, x2);
	float xmax = max(x1, x2);*/

	if (x1 > x2) swap(x1, x2);

	x1 = max((float)0, x1);
	x2 = min((float)gWidth, x2);

	for (float x = x1; x < x2; x+=1)
	{
		if (x < 0 || x >= gWidth) continue;
		if (PixelShader)
		{
			// prepare pixel shader inputs
			shader_in::screen_pos = Vector2(x / gWidth, (float)y / gHeight);

			auto out = PixelShader() * 255;
			_PutPixel(round(x), y, round(out.x), round(out.y), round(out.z), round(out.w));
		}
		else
		{
			_PutPixel(round(x), y, 255, 255, 255, 255);
		}
	}
}

struct Edge
{
	Edge(Vector2 l, Vector2 r) : a(l), b(r) {};
	Edge(Vector4 l, Vector4 r) : a(l.x, l.y), b(r.x, r.y) {};
	Vector2 a;
	Vector2 b;
};

static FORCEINLINE void _DrawSpansBetweenEdges(Edge l, Edge s)
{
	bool drewAThing = false;

	// edge coordinate swapping if necessary (dy need to be positive)
	if (s.a.y > s.b.y) swap(s.a, s.b);
	if (l.a.y > l.b.y) swap(l.a, l.b);

	float lydiff = l.b.y - l.a.y;
	if (lydiff == 0) return;

	float sydiff = s.b.y - s.a.y;
	if (sydiff == 0) return;

	float lxdiff = l.b.x - l.a.x;
	float sxdiff = s.b.x - s.a.x;

	float factor1 = (s.a.y - l.a.y) / lydiff;
	float factorStep1 = 1.f / lydiff;
	float factor2 = 0;
	float factorStep2 = 1.f / sydiff;

	float maxY = min((float)gHeight, s.b.y);

	for (float y = s.a.y; y < maxY; y+=1)
	{
		drewAThing = true;
		_DrawSpan(
			l.a.x + (lxdiff * factor1),
			s.a.x + (sxdiff * factor2),
			round(y));

		//FlushAndRefresh();
		//PollEvents();
		//if (!(y < 0 || y >= gHeight)) SDL_Delay(2);
		factor1 += factorStep1;
		factor2 += factorStep2;
	}
	if (drewAThing == false) fprintf(debugOut, "warning: didn't draw any spans for tri %d\n", triangleCounter);
}

void DrawTriangle(Vector4 v1, Vector4 v2, Vector4 v3)
{
	// transform
	Matrix4 pvm = projection * view * model;
	//pvm = model * view * projection;
	v1 = pvm * v1;
	v2 = pvm * v2;
	v3 = pvm * v3;

	v1 = resolution * v1;
	v2 = resolution * v2;
	v3 = resolution * v3;

	if (v1.w != 0) v1 /= v1.w; else v1 = Vector4();
	if (v2.w != 0) v2 /= v2.w; else v2 = Vector4();
	if (v3.w != 0) v3 /= v3.w; else v2 = Vector4();

	fprintf(debugOut, "Triangle %d start\n", triangleCounter);
	fprintf(debugOut, "%f . %f\n", v1.x, v1.y);
	fprintf(debugOut, "%f . %f\n", v2.x, v2.y);
	fprintf(debugOut, "%f . %f\n", v3.x, v3.y);
	fprintf(debugOut, "-----------------------\n");
	//v1 = v1 * rpvm;
	//v2 = v2 * rpvm;
	//v3 = v3 * rpvm;

	Edge e[3] = {
		Edge(v1, v2),
		Edge(v2, v3),
		Edge(v3, v1),
	};

	float maxlen = 0;
	int longEdge = 0;

	for (int i = 0; i < 3; i++)
	{
		float len = abs(e[i].b.y - e[i].a.y);
		if (len >= maxlen)
		{
			maxlen = len;
			longEdge = i;
		}
	}

	int shortEdge1 = (longEdge + 1) % 3;
	int shortEdge2 = (longEdge + 2) % 3;

	_DrawSpansBetweenEdges(e[longEdge], e[shortEdge1]);
	_DrawSpansBetweenEdges(e[longEdge], e[shortEdge2]);
	triangleCounter++;
}