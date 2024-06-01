#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct DirectionalLight{
    Vector4 color_;
    Vector3 direction_;
    float intensity;
};
