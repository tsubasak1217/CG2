#pragma once
#include "Transform.h"
#include "Vector2.h"
#include <stdint.h>

struct Camera{

    Camera();
    ~Camera();
    void Update();

    Transform transform_;
    Vector2 clipRange_;
    uint32_t projectionMode_;
    float znear_;
    float zfar_;

    Matrix4x4 worldMat_;
    Matrix4x4 viewMat_;
    Matrix4x4 projectionMat_;
    Matrix4x4 projectionMat2D_;
    Matrix4x4 viewProjectionMat_;
    Matrix4x4 viewProjectionMat2D_;

    Vector3 normal_;
};

enum PROJECTIONMODE{
    PERSPECTIVE,
    ORTHO
};