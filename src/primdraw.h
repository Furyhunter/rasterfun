#pragma once

#include "vec.h"
#include "matrix.h"

#include <functional>


void BeginDraw();
void EndDraw();
void DrawTriangle(Vector4 p1, Vector4 p2, Vector4 p3);
void SetShader(std::function<Vector4()> func);

namespace shader_in
{
	extern Vector4 vec;
	extern Vector4 color;
	extern Matrix4 model;
	extern Matrix4 view;
	extern Matrix4 projection;
	extern Matrix4 resolution;

	extern Vector2 screen_pos;
};
