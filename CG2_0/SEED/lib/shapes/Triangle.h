#pragma once
#include "Vector3.h"
#include <stdint.h>

struct Triangle{
    Vector3 localVertex[3];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    uint32_t color;

    Triangle(){};
    Triangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        uint32_t color = 0xffffffff
    ){
        localVertex[0] = v1;
        localVertex[1] = v2;
        localVertex[2] = v3;
        this->scale = scale;
        this->rotate = rotate;
        this->translate = translate;
        this->color = color;
    }
};