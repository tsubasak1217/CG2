#include "MyMath.h"
#include <cmath>
#include <assert.h>

//================================================================
//                      数学的な関数
//================================================================

/*---------------------------- 長さを計る関数 ----------------------------*/

float MyMath::Length(const Vector2& pos1, const Vector2& pos2)
{
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    return std::sqrt(xLength * xLength + yLength * yLength);
}

float MyMath::Length(const Vector3& pos1, const Vector3& pos2)
{
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    float zLength = (pos1.z - pos2.z);
    return std::sqrt(xLength * xLength + yLength * yLength + zLength * zLength);
}

float MyMath::Length(const Vector2& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float MyMath::Length(const Vector3& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

/*----------------------- ベクトルを正規化する関数 ------------------------*/

Vector2 MyMath::Normalize(const Vector2& vec)
{
    float length = Length(vec);
    return length != 0 ? vec / Length(vec) : Vector2(0.0f, 0.0f);
}

Vector3 MyMath::Normalize(const Vector3& vec)
{
    float length = Length(vec);
    return length != 0 ? vec / Length(vec) : Vector3(0.0f, 0.0f, 0.0f);
}

/*-------------------------- 内積を求める関数 ---------------------------*/


float MyMath::Dot(const Vector2& a, const Vector2& b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float MyMath::Dot(const Vector3& a, const Vector3& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float MyMath::Dot(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos)
{
    return Dot(pos2 - pos1, targetPos - pos1);
}

float MyMath::Dot(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos)
{
    return Dot(pos2 - pos1, targetPos - pos1);
}


float MyMath::DotNormal(const Vector2& vec1, const Vector2& vec2)
{
    float length = Length(vec1);
    return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
}

float MyMath::DotNormal(const Vector3& vec1, const Vector3& vec2)
{
    float length = Length(vec1);
    return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
}

float MyMath::DotNormal(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos)
{
    Vector2 lineVector = pos2 - pos1;
    Vector2 forTarget = targetPos - pos1;
    float lineLength = Length(lineVector);

    return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
}


float MyMath::DotNormal(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos)
{
    Vector3 lineVector = pos2 - pos1;
    Vector3 forTarget = targetPos - pos1;
    float lineLength = Length(lineVector);

    return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
}

/*-------------------------- 外積を求める関数 ---------------------------*/

float MyMath::Cross(const Vector2& vec1, const Vector2& vec2)
{
    return vec1.x * vec2.y - vec1.y * vec2.x;
}

Vector3 MyMath::Cross(const Vector3& vec1, const Vector3& vec2, bool kViewMode)
{
    if(kViewMode == kScreen) {
        return Vector3(
            -vec1.y * vec2.z - vec1.z * -vec2.y,
            vec1.z * vec2.x - vec1.x * vec2.z,
            vec1.x * -vec2.y - -vec1.y * vec2.x
        );
    }

    return Vector3(
        vec1.y * vec2.z - vec1.z * vec2.y,
        vec1.z * vec2.x - vec1.x * vec2.z,
        vec1.x * vec2.y - vec1.y * vec2.x
    );
}

float MyMath::Cross(const Vector2& originPos, const Vector2& endPos, const Vector2& targetPos)
{
    float length = Length(endPos - originPos);
    return length != 0 ? Cross(endPos - originPos, targetPos - originPos) / length : 0.0f;
}

/*---------------------- 射影ベクトルを求める関数 -----------------------*/

Vector2 MyMath::ProjectVec(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos)
{
    Vector2 vec = pos2 - pos1;
    return vec * DotNormal(pos1, pos2, targetPos);
}

Vector3 MyMath::ProjectVec(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos)
{
    Vector3 vec = pos2 - pos1;
    return vec * DotNormal(pos1, pos2, targetPos);
}

Vector2 MyMath::ProjectVec(const Vector2& vec1, const Vector2& vec2)
{
    return vec1 * DotNormal(vec1, vec2);
}

Vector3 MyMath::ProjectVec(const Vector3& vec1, const Vector3& vec2)
{
    return vec1 * DotNormal(vec1, vec2);
}

/*---------------------- 直線に対する最近傍点を求める関数 -----------------------*/

Vector2 MyMath::ClosestPoint(const Vector2& seg_origin, const Vector2& seg_end, const Vector2& point)
{
    return seg_origin + ProjectVec(point - seg_origin, seg_end - seg_origin);
}

Vector3 MyMath::ClosestPoint(const Vector3& seg_origin, const Vector3& seg_end, const Vector3& point)
{
    return seg_origin + ProjectVec(point - seg_origin, seg_end - seg_origin);
}

//================================================================
//                      個人用な便利関数
//================================================================

// 負数を0にする関数
uint32_t MyMath::negaZero(int32_t num){ return num > 0 ? num : 0; }
float MyMath::negaZero(float num){ return num > 0.0f ? num : 0.0f; }

//================================================================
//                        色を扱う関数
//================================================================

// 色の各成分を求める関数
uint32_t MyMath::Red(uint32_t color){ return (color >> 24) & 0xFF; }
uint32_t MyMath::Green(uint32_t color){ return (color >> 16) & 0xFF; }
uint32_t MyMath::Blue(uint32_t color){ return (color >> 8) & 0xFF; }
uint32_t MyMath::Alpha(uint32_t color){ return color & 0xFF; }

// RGBA形式のカラーコードをグレースケールに変換する関数
uint32_t MyMath::GrayScale(uint32_t color)
{
    /*
            真っ白のとき(RGB最大値のとき)が最大値の"1"だとして
          そのときRGBはそれぞれどの程度白く見えますかというのが重み

                R: 0.2126    G: 0.7152   B: 0.0722

            RGBが合わさると白になるので、上の合計値は1になる。
            グレースケールはRGBがすべて同じ値になる必要がある。
              明度のイメージを損なわないように変換するには

               "今の色 * 重み" で各色の補正後の色を求め
             すべての要素を足すことで0~1で平均明度を得られる
        カラーコードの各色を平均明度にすることでグレースケールが求まる
    */
    float trancedRed = float(Red(color)) * 0.2126f;
    float trancedGreen = float(Green(color)) * 0.7152f;
    float trancedBlue = float(Blue(color)) * 0.0722f;

    int gray = int(trancedRed + trancedGreen + trancedBlue);

    return 0xFF + (gray << 24) + (gray << 16) + (gray << 8);
}


// RGBA形式のカラーコードをVector4形式に変換する関数 (各要素は0~1に収まる)
Vector4 MyMath::FloatColor(uint32_t color)
{
    float delta = 1.0f / 255.0f;

    Vector4 colorf = {
        float(Red(color)) * delta,
        float(Green(color)) * delta,
        float(Blue(color)) * delta,
        float(Alpha(color)) * delta
    };

    return colorf;
}

uint32_t MyMath::IntColor(const Vector4& color)
{
    uint32_t red = std::clamp(int(color.x * 255.0f), 0, 255) << 24;
    uint32_t green = std::clamp(int(color.y * 255.0f), 0, 255) << 16;
    uint32_t blue = std::clamp(int(color.z * 255.0f), 0, 255) << 8;
    uint32_t alpha = std::clamp(int(color.w * 255.0f), 0, 255);

    return red + green + blue + alpha;
}

uint32_t MyMath::HSV_to_RGB(float h, float s, float v, float alpha)
{

    // 彩度が0なので明度のみを反映
    if(s == 0.0) {
        return IntColor(Vector4(v, v, v, alpha));
    }

    h *= 6.0;
    int i = int(h);
    float f = h - i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    if(i % 6 == 0) {
        return  IntColor(Vector4(v, t, p,alpha));
    } else if(i % 6 == 1) {
        return  IntColor(Vector4(q, v, p,alpha));
    } else if(i % 6 == 2) {
        return  IntColor(Vector4(p, v, t,alpha));
    } else if(i % 6 == 3) {
        return  IntColor(Vector4(p, q, v,alpha));
    } else if(i % 6 == 4) {
        return  IntColor(Vector4(t, p, v,alpha));
    } else {
        return  IntColor(Vector4(v, p, q,alpha));
    }
}
