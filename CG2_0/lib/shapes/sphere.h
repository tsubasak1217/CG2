#pragma once
#include "Vector4.h"
#include <vector>

struct Sphere{
    Sphere(int kSubdivision);
public:
    std::vector<std::vector<Vector4>>vertexes_;
};