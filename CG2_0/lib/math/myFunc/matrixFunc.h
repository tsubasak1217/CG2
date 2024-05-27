#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

//================================================================
//                      行列の計算関数
//================================================================

// -----------------加算------------------
Matrix2x2 Add(const Matrix2x2& matrix1, const Matrix2x2& matrix2);
Matrix3x3 Add(const Matrix3x3& matrix1, const Matrix3x3& matrix2);
Matrix4x4 Add(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

// -----------------減算------------------
Matrix2x2 Subtract(const Matrix2x2& matrix1, const Matrix2x2& matrix2);
Matrix3x3 Subtract(const Matrix3x3& matrix1, const Matrix3x3& matrix2);
Matrix4x4 Subtract(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

// ---------------積を求める----------------
Vector2 Multiply(const Vector2& vector, const Matrix2x2& matrix);
Vector2 Multiply(const Vector2& vector, const Matrix3x3& matrix);
Matrix2x2 Multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2);
Matrix3x3 Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2);
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);
// --------------スカラー倍----------------
Vector2 Multiply(const Vector2& vector, float scalar);
Vector3 Multiply(const Vector3& vector, float scalar);
Matrix2x2 Multiply(const Matrix2x2& matrix, float scalar);
Matrix3x3 Multiply(const Matrix3x3& matrix, float scalar);
Matrix4x4 Multiply(const Matrix4x4& matrix, float scalar);

// -----------------割り算------------------
Matrix2x2 Devide(const Matrix2x2& matrix, float devideNum);
Matrix3x3 Devide(const Matrix3x3& matrix, float devideNum);
Matrix4x4 Devide(const Matrix4x4& matrix, float devideNum);

// 同時座標系からデカルト座標系に変換する関数
Vector2 TransformDescartes(const Vector2& vector, const Matrix3x3& matrix);
Vector3 TransformDescartes(const Vector3& vector, const Matrix4x4& matrix);

// 単位行列を返す関数
Matrix2x2 IdentityMat2();
Matrix3x3 IdentityMat3();
Matrix4x4 IdentityMat4();

// 拡大縮小行列を作る関数
Matrix3x3 ScaleMatrix(float scaleX, float scaleY);
Matrix3x3 ScaleMatrix(const Vector2& scale);
Matrix4x4 ScaleMatrix(float scaleX, float scaleY, float scaleZ);
Matrix4x4 ScaleMatrix(const Vector3& scale);

// 回転行列を作る関数
Matrix3x3 RotateMatrix(float theta);
Matrix4x4 RotateMatrix(const Vector3& rotate);

// 平行移動行列を作る関数
Matrix3x3 TranslateMatrix(float tx, float ty);
Matrix3x3 TranslateMatrix(const Vector2& t);
Matrix4x4 TranslateMatrix(float tx, float ty, float tz);
Matrix4x4 TranslateMatrix(const Vector3& t);

// アフィン変換行列を作る関数
Matrix3x3 AffineMatrix(Vector2 scale, float rotateTheta, Vector2 transLate);
Matrix4x4 AffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//逆行列を求める関数
Matrix2x2 InverseMatrix(const Matrix2x2& matrix);
Matrix3x3 InverseMatrix(const Matrix3x3& matrix);
Matrix4x4 InverseMatrix(const Matrix4x4& matrix);

//転置行列を求める関数
Matrix2x2 Transpose(const Matrix2x2& matrix);
Matrix3x3 Transpose(const Matrix3x3& matrix);
Matrix4x4 Transpose(const Matrix4x4& matrix);

//正射影行列を求める関数
Matrix3x3 OrthoMatrix(float left, float right, float top, float bottom);
Matrix4x4 OrthoMatrix(float left, float right, float top, float bottom, float znear, float zfar);

// 透視投影行列(視錐台)を求める関数
float AspectRatio(float windowWidth, float windowHeight);
Matrix4x4 PerspectiveMatrix(float fovY, float aspectRatio, float znear, float zfar);

//ビューポート変換行列を求める関数
Matrix3x3 ViewportMatrix(const Vector2& size, const Vector2& LeftTop);
Matrix4x4 ViewportMatrix(const Vector2& size, const Vector2& LeftTop, float minDepth, float maxDepth);

//レンダリングパイプライン作る関数
Matrix3x3 WvpVpMatrix(
    Vector2 playerPos, Vector2 playerScale, float playerAngle,
    Vector2 cameraPos, Vector2 cameraScale, float cameraAngle,
    Vector2 cameraRange, Vector2 leftTop,
    Vector2 screenSize
);