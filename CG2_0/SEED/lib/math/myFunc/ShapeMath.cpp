#include "ShapeMath.h"

//---------------------- 正三角形 -------------------------

Triangle MakeEqualTriangle(float radius, uint32_t color){

    float devideTheta = (std::numbers::pi_v<float> *2.0f) / 3.0f;
    float startTheta = std::numbers::pi_v<float> *0.5f;

    return Triangle(
        { radius * std::cos(startTheta),radius * std::sin(startTheta),0.0f },
        { radius * std::cos(startTheta + devideTheta * 2.0f),radius * std::sin(startTheta + devideTheta * 2.0f),0.0f },
        { radius * std::cos(startTheta + devideTheta),radius * std::sin(startTheta + devideTheta),0.0f },
        { 1.0f,1.0f,1.0f },
        { 0.0f,0.0f,0.0f },
        { 0.0f,0.0f,0.0f },
        color
    );
}

Triangle2D MakeEqualTriangle2D(float radius, uint32_t color){

    float devideTheta = (std::numbers::pi_v<float> *2.0f) / 3.0f;
    float startTheta = std::numbers::pi_v<float> * 0.5f * -1.0f;

    return Triangle2D(
        { radius * std::cos(startTheta),radius * std::sin(startTheta) },
        { radius * std::cos(startTheta + devideTheta),radius * std::sin(startTheta + devideTheta) },
        { radius * std::cos(startTheta + devideTheta * 2.0f),radius * std::sin(startTheta + devideTheta * 2.0f) },
        { 1.0f,1.0f },
        0.0f,
        { 0.0f,0.0f },
        color
    );
}