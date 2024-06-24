#include "Camera.h"
#include "MatrixFunc.h"
#include "MyMath.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
    // カメラのワールド行列
    worldMat_ = AffineMatrix(
        transform_.scale_,
        transform_.rotate_,
        transform_.translate_
    );

    // カメラの逆行列
    viewMat_ = InverseMatrix(worldMat_);

    //射影行列の生成
    projectionMat_ = PerspectiveMatrix(
        0.45f,
        clipRange_.x / clipRange_.y,
        znear_, zfar_
    );

    projectionMat2D_ = OrthoMatrix(
        0.0f, clipRange_.x,
        0.0f, clipRange_.y,
        zfar_, znear_
    );

    // カメラ法線
    normal_ = { 0.0f,0.0f,1.0f };
    normal_ = MyMath::Normalize(Multiply(normal_, worldMat_));

    // ViewProjectionMatrixの計算
    viewProjectionMat_ = Multiply(viewMat_, projectionMat_);
    viewProjectionMat2D_ = Multiply(viewMat_, projectionMat2D_);

}
