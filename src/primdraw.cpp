#include "primdraw.h"

#include "common.h"

#include "matrix.h"

#include <algorithm>
#include <functional>

using namespace std;

static Matrix4 projection;

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

static inline s32 orient2d(Vector3 a, Vector3 b, s32 px, s32 py)
{
	return (b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x);
}

void BeginDraw()
{
	projection = setFrustum(45, (gWidth/gHeight), .1f, 1000.f);
	SDL_LockSurface(gMainSurface);
}

void EndDraw()
{
	SDL_UnlockSurface(gMainSurface);
}

void SetShader(function<Vector4()> func)
{
	PixelShader = func;
}

void DrawTriangle(Vector3 v1, Vector3 v2, Vector3 v3)
{

	s32 minX = round(min(v1.x, min(v2.x, v3.x)));
	s32 minY = round(min(v1.y, min(v2.y, v3.y)));
	s32 maxX = round(max(v1.x, max(v2.x, v3.x)));
	s32 maxY = round(max(v1.y, max(v2.y, v3.y)));

	// screen bounds
	minX = max(minX, 0);
	minY = max(minY, 0);
	maxX = min(maxX, (s32)gWidth);
	maxY = min(maxY, (s32)gHeight);

	s32 px, py;
	for (py = minY; py <= maxY; py++)
	{
		for (px = minX; px <= maxX; px++)
		{
			s32 w0 = orient2d(v2, v3, px, py);
			s32 w1 = orient2d(v3, v1, px, py);
			s32 w2 = orient2d(v1, v2, px, py);

			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
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
}