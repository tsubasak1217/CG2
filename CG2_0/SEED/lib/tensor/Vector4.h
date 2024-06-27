#pragma once
#include <assert.h>

struct Vector3;

struct Vector4 final {
    float x;
    float y;
    float z;
    float w;

    Vector4(){};
    Vector4(float x, float y, float z, float w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    ///////////////////////////////////////////////////////////////////
    /*                            Transform                          */
    ///////////////////////////////////////////////////////////////////

    Vector3 ToVec3();
};