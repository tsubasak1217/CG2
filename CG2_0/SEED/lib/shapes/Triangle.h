#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include <stdint.h>
#include <cmath>
#include <numbers>
#include "MyMath.h"

struct Triangle{
    Vector3 localVertex[3];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    uint32_t color;
    Vector4 colorf;

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
        colorf = MyMath::FloatColor(color);
    }
};

struct Triangle2D{
    Vector2 localVertex[3];
    Vector2 scale;
    float rotate;
    Vector2 translate;
    uint32_t color;
    Vector4 colorf;

    Triangle2D(){};
    Triangle2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3,
        const Vector2& scale = { 1.0f,1.0f },
        float rotate = 0.0f,
        const Vector2& translate = { 0.0f,0.0f },
        uint32_t color = 0xffffffff
    ){
        localVertex[0] = v1;
        localVertex[1] = v2;
        localVertex[2] = v3;
        this->scale = scale;
        this->rotate = rotate;
        this->translate = translate;
        this->color = color;
        colorf = MyMath::FloatColor(color);
    }
};