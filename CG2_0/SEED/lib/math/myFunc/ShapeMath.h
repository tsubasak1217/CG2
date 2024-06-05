#pragma once
#include "include_shapes.h"
#include "Vector3.h"
#include "Vector2.h"
#include <stdint.h>
#include <cmath>
#include <numbers>

//---------------------- 正三角形 -------------------------

Triangle MakeEqualTriangle(float radius, uint32_t color);
Triangle2D MakeEqualTriangle2D(float radius, uint32_t color);