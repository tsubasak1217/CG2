#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Transform {
    Vector3 scale_;
    Vector3 rotate_;
    Vector3 translate_;

    Matrix4x4 WVP_;
    Matrix4x4 world_;
};

struct TransformMatrix {
    Matrix4x4 WVP_;
    Matrix4x4 world_;
};