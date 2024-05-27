#include "sphere.h"
#include <cmath>
#include <numbers>

Sphere::Sphere(int kSubdivision)
{
    float pi = (float)std::numbers::pi;

    float kLatEvery_ = pi / kSubdivision;// 縦の1刻みごとの角度
    float kLonEvery_ = (2.0f * pi) / kSubdivision;// 横の1刻みごとの角度

    float r1 = 0.5f;// 横の半径

    // 結果を格納する変数
    Vector4 vertex;

    // 計算に使う値や、結果の出ているY値などを先に求める
    std::vector<float>posY(0, 0.0f);
    std::vector<float>r2(0, 0.0f);
    for(int latIdx = 0; latIdx <= kSubdivision; ++latIdx){
        posY.push_back(r1 * std::sin(-(pi * 0.5f) + kLatEvery_ * latIdx));
        r2.push_back(r1 * std::cos(-(pi * 0.5f) + kLatEvery_ * latIdx));
    }

    // X,Z座標を求めていく
    for(int latIdx = 0; latIdx <= kSubdivision; ++latIdx){
        std::vector<Vector4>rowVertexes;

        for(int lonIdx = 0; lonIdx <= kSubdivision; ++lonIdx){

            // 各座標を求める
            vertex.x = r2[latIdx] * std::cos(kLonEvery_ * lonIdx);
            vertex.y = posY[latIdx] * -1.0f;
            vertex.z = r2[latIdx] * std::sin(kLonEvery_ * lonIdx);
            vertex.w = 1.0f;

            rowVertexes.push_back(vertex);
        }

        vertexes_.push_back(rowVertexes);
    }
}
