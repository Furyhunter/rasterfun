#pragma once

#include "vec.h"

#include <functional>

void BeginDraw();
void EndDraw();
void DrawTriangle(Vector3 p1, Vector3 p2, Vector3 p3);
void SetShader(std::function<Vector4()> func);
