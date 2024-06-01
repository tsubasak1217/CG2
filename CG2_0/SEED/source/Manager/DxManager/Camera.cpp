#include "Camera.h"
#include "MatrixFunc.h"

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

    //射影行列の生成
    if(projectionMode_ == PERSPECTIVE){

        projectionMat_ = PerspectiveMatrix(
            0.45f,
            clipRange_.x / clipRange_.x,
            znear_, zfar_
        );

    } else{

        projectionMat_ = OrthoMatrix(
            0.0f, clipRange_.x,
            0.0f, clipRange_.y,
            zfar_, znear_
        );
    }

    // ViewProjectionMatrixの計算
    viewMat_ = InverseMatrix(worldMat_);
    viewProjectionMat_ = Multiply(viewMat_, projectionMat_);
}
