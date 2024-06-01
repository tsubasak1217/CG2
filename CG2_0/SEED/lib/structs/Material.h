#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"

struct Material{
    Vector4 color_;
    int enableLighting_;
    float padding_[3];
    Matrix4x4 uvTransform_;
};