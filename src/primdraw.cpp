#include "primdraw.h"

#include "common.h"

#include "matrix.h"

#include <algorithm>
#include <functional>

using namespace std;

namespace shader_in
{
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 resolution;
};

using namespace shader_in;

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
	model = Matrix4().identity();
	view = Matrix4().identity();
	resolution = Matrix4().identity();
	projection = Matrix4().identity();

	//projection = setOrthoFrustum(-1, 1, -1, 1, -1, 1);
	projection = setFrustum(45, ((float)gWidth/gHeight), .001f, 1000);
	resolution.translate(1, 1, 0);
	resolution.scale(gWidth/2, gHeight/2, 1);
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

void DrawTriangle(Vector4 v1, Vector4 v2, Vector4 v3)
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
				Vector4 color;
				if (PixelShader)
					color = PixelShader();
				else
					color = Vector4(1,0,0,1);
				u8 * pixels = (u8*)gMainSurface->pixels;
				pixels[(px + ((gHeight-py) * gMainSurface->w))*4 + 0] = round(color.w * 255);
				pixels[(px + ((gHeight-py) * gMainSurface->w))*4 + 1] = round(color.z * 255);
				pixels[(px + ((gHeight-py) * gMainSurface->w))*4 + 2] = round(color.y * 255);
				pixels[(px + ((gHeight-py) * gMainSurface->w))*4 + 3] = round(color.x * 255);
			}
		}
	}
}