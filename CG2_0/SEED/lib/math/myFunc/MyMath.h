#pragma once
#include "include_tensors.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

class MyMath{
public:

    //================================================================
    //                      数学的な関数
    //================================================================

    /*---------------------------- 長さを計る関数 ----------------------------*/

    // 2点間の距離を計算する関数 (2D)
    static float Length(const Vector2& pos1, const Vector2& pos2);
    // 2点間の距離を計算する関数 (3D)
    static float Length(const Vector3& pos1, const Vector3& pos2);
    // ベクトルの長さを返す関数 (2D)
    static float Length(const Vector2& vec);
    // ベクトルの長さを返す関数 (3D)
    static float Length(const Vector3& vec);

    /*----------------------- ベクトルを正規化する関数 ------------------------*/

    // ベクトルを正規化する関数 (2D)
    static Vector2 Normalize(const Vector2& vec);
    // ベクトルを正規化する関数 (3D)
    static Vector3 Normalize(const Vector3& vec);

    /*-------------------------- 内積を求める関数 ---------------------------*/

    // 2ベクトルの内積を求める関数 (2D)
    static float Dot(const Vector2& vec1, const Vector2& vec2);
    // 2ベクトルの内積を求める関数 (3D)
    static float Dot(const Vector3& vec1, const Vector3& vec2);
    // 3点の座標を指定して内積を求める関数 (2D)
    static float Dot(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos);
    // 3点の座標を指定して内積を求める関数 (3D)
    static float Dot(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos);
    // 2ベクトルの内積を求める関数 (長さで正規化する) (2D)
    static float DotNormal(const Vector2& vec1, const Vector2& vec2);
    // 2ベクトルの内積を求める関数 (長さで正規化する) (3D)
    static float DotNormal(const Vector3& vec1, const Vector3& vec2);
    // 3点の座標を指定して内積を求める関数 (長さで正規化する) (2D)
    static float DotNormal(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos);
    // 3点の座標を指定して内積を求める関数 (長さで正規化する) (3D)
    static float DotNormal(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos);

    /*-------------------------- 外積を求める関数 ---------------------------*/

    // 2ベクトルの外積を求める関数 (2D)
    static float Cross(const Vector2& vec1, const Vector2& vec2);
    // 2ベクトルの外積を求める関数 (3D)
    static Vector3 Cross(const Vector3& vec1, const Vector3& vec2, bool kViewMode);
    // 3点の座標を指定して外積を求める関数 (長さで正規化する) (2D)
    static float Cross(const Vector2& originPos,const Vector2& endPos,const Vector2& targetPos);

    /*----------------------- 射影ベクトルを求める関数 -----------------------*/

    // 3点の座標を指定して射影ベクトルを求める関数 (2D)
    static Vector2 ProjectVec(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos);
    // 3点の座標を指定して射影ベクトルを求める関数 (3D)
    static Vector3 ProjectVec(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos);
    // 2ベクトルから射影ベクトルを求める関数 (2D)
    static Vector2 ProjectVec(const Vector2& vec1, const Vector2& vec2);
    // 2ベクトルから射影ベクトルを求める関数 (3D)
    static Vector3 ProjectVec(const Vector3& vec1, const Vector3& vec2);

    /*-------------------- 線への最近傍点を求めるを求める関数 ------------------*/

    // 直線への最近傍点を求める関数 (2D)
    static Vector2 ClosestPoint(const Vector2& seg_origin, const Vector2& seg_end, const Vector2& point);
    // 直線への最近傍点を求める関数 (3D)
    static Vector3 ClosestPoint(const Vector3& seg_origin, const Vector3& seg_end, const Vector3& point);


    //================================================================
    //                      個人用な便利関数
    //================================================================

    // 負数を0に変換する関数 (int)
    static uint32_t negaZero(int32_t num);
    // 負数を0に変換する関数 (float)
    static float negaZero(float num);

    //================================================================
    //                        色を扱う関数
    //================================================================

    /*-------------------- 色の各成分を求める関数 ------------------*/

    // RGBA形式のカラーコードからREDの成分を得る関数
    static uint32_t Red(uint32_t color);
    // RGBA形式のカラーコードからGREENの成分を得る関数
    static uint32_t Green(uint32_t color);
    // RGBA形式のカラーコードからBLUEの成分を得る関数
    static uint32_t Blue(uint32_t color);
    // RGBA形式のカラーコードからALPHA(透明度)の成分を得る関数
    static uint32_t Alpha(uint32_t color);

    // グレースケールを求める関数
    static uint32_t GrayScale(uint32_t color);

    // カラーコードからVector4に変換する関数 (0~1に収められる)
    static Vector4 FloatColor(uint32_t color);
    // Vector4からカラーコードに変換する関数
    static uint32_t IntColor(const Vector4& color);

    // HSVをRGBに変換する関数
    static uint32_t HSV_to_RGB(float h, float s, float v,float alpha);
};

enum VIEWMODE {
    kScreen,
    kWorld
};