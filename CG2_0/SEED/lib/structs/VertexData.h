#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <stdint.h>

struct VertexData {
    Vector4 position_;
    Vector4 color_;
    Vector2 texcoord_;
    Vector3 normal_;
    Matrix4x4 wvp_;
    Matrix4x4 world_;
};