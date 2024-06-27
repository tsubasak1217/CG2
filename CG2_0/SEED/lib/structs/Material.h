#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"
#include <stdint.h>
#include <string>

struct Material{
    Vector4 color_;
    Matrix4x4 uvTransform_;
    int enableLighting_;
};

struct MaterialData{
    std::string textureFilePath_;
};