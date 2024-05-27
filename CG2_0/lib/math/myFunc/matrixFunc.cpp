#include "matrixFunc.h"
#include <cmath>
#include <cassert>

//================================================================
//                        行列関数
//================================================================

// 加算
Matrix2x2 Add(const Matrix2x2& matrix1, const Matrix2x2& matrix2) {

    Matrix2x2 result;

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
};
Matrix3x3 Add(const Matrix3x3& matrix1, const Matrix3x3& matrix2) {

    Matrix3x3 result;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
}

Matrix4x4 Add(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {

    Matrix4x4 result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
}

// 減算
Matrix2x2 Subtract(const Matrix2x2& matrix1, const Matrix2x2& matrix2) {

    Matrix2x2 result;

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};
Matrix3x3 Subtract(const Matrix3x3& matrix1, const Matrix3x3& matrix2) {

    Matrix3x3 result;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};

Matrix4x4 Subtract(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {

    Matrix4x4 result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};

// -----------------割り算------------------
Matrix2x2 Devide(const Matrix2x2& matrix, float devideNum) {
    Matrix2x2 result;

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
};

Matrix3x3 Devide(const Matrix3x3& matrix, float devideNum) {

    Matrix3x3 result;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
}

Matrix4x4 Devide(const Matrix4x4& matrix, float devideNum) {

    Matrix4x4 result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
};

// ---------------積を求める----------------
Vector2 Multiply(const Vector2& vector, const Matrix2x2& matrix) {

    return {
        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0],
        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1]
    };
};
Vector2 Multiply(const Vector2& vector, const Matrix3x3& matrix) {
    Vector2 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
    w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];

    assert(w != 0);

    result.x /= w;
    result.y /= w;

    return result;
};

Matrix2x2 Multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2) {

    Matrix2x2 result;

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {

            result.m[i][j] =
                (matrix1.m[i][0] * matrix2.m[0][j]) +
                (matrix1.m[i][1] * matrix2.m[1][j]);
        }
    }

    return result;
};
Matrix3x3 Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) {

    Matrix3x3 result;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {

            result.m[i][j] =
                (matrix1.m[i][0] * matrix2.m[0][j]) +
                (matrix1.m[i][1] * matrix2.m[1][j]) +
                (matrix1.m[i][2] * matrix2.m[2][j]);
        }
    }

    return result;
}

Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {

    Matrix4x4 result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {

            result.m[i][j] =
                (matrix1.m[i][0] * matrix2.m[0][j]) +
                (matrix1.m[i][1] * matrix2.m[1][j]) +
                (matrix1.m[i][2] * matrix2.m[2][j]) +
                (matrix1.m[i][3] * matrix2.m[3][j]);
        }
    }

    return result;
}

// --------------スカラー倍----------------
Vector2 Multiply(const Vector2& vector, float scalar) {
    return { vector.x * scalar,vector.y * scalar };
}
Vector3 Multiply(const Vector3& vector, float scalar) {
    return { vector.x * scalar,vector.y * scalar,vector.z * scalar };
}
Matrix2x2 Multiply(const Matrix2x2& matrix, float scalar) {

    Matrix2x2 result;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}
Matrix3x3 Multiply(const Matrix3x3& matrix, float scalar) {

    Matrix3x3 result;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}
Matrix4x4 Multiply(const Matrix4x4& matrix, float scalar) {

    Matrix4x4 result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}

// 同時座標系からデカルト座標系に変換する関数
Vector2 TransformDescartes(const Vector2& vector, const Matrix3x3& matrix) {
    Vector2 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + matrix.m[2][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + matrix.m[2][1];
    w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + matrix.m[2][2];

    assert(w != 0);

    result.x /= w;
    result.y /= w;

    return result;
}

Vector3 TransformDescartes(const Vector3& vector, const Matrix4x4& matrix) {
    Vector3 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
    w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

    assert(w != 0);

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}


// 単位行列を返す関数
Matrix2x2 IdentityMat2() {

    Matrix2x2 identity(
        1.0f, 0.0f,
        0.0f, 1.0f
    );

    return identity;
}

Matrix3x3 IdentityMat3() {

    Matrix3x3 identity(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    return identity;
}

Matrix4x4 IdentityMat4() {

    Matrix4x4 identity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return identity;
}

/*------------------------- 拡大縮小行列を作る関数 --------------------------*/
Matrix3x3 ScaleMatrix(float scaleX, float scaleY) {
    Matrix3x3 matrix;
    matrix.m[0][0] = scaleX;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scaleY;
    matrix.m[1][2] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix3x3 ScaleMatrix(const Vector2& scale) {
    Matrix3x3 matrix;
    matrix.m[0][0] = scale.x;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scale.y;
    matrix.m[1][2] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix4x4 ScaleMatrix(float scaleX, float scaleY, float scaleZ) {
    Matrix4x4 matrix;
    matrix.m[0][0] = scaleX;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][3] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scaleY;
    matrix.m[1][2] = 0;
    matrix.m[1][3] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = scaleZ;
    matrix.m[2][3] = 0;

    matrix.m[3][0] = 0;
    matrix.m[3][1] = 0;
    matrix.m[3][2] = 0;
    matrix.m[3][3] = 1;

    return matrix;
}

Matrix4x4 ScaleMatrix(const Vector3& scale) {
    Matrix4x4 matrix;
    matrix.m[0][0] = scale.x;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][3] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scale.y;
    matrix.m[1][2] = 0;
    matrix.m[1][3] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = scale.z;
    matrix.m[2][3] = 0;

    matrix.m[3][0] = 0;
    matrix.m[3][1] = 0;
    matrix.m[3][2] = 0;
    matrix.m[3][3] = 1;

    return matrix;
}

/*------------------------- 回転行列を作る関数 --------------------------*/
Matrix3x3 RotateMatrix(float theta) {

    Matrix3x3 result;
    result.m[0][0] = std::cos(theta);
    result.m[0][1] = std::sin(theta);
    result.m[0][2] = 0.0f;

    result.m[1][0] = -std::sin(theta);
    result.m[1][1] = std::cos(theta);
    result.m[1][2] = 0.0f;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;

    return result;
}

Matrix4x4 RotateMatrix(const Vector3& rotate) {
    Matrix4x4 rotateMat[3];

    /*-------X軸の回転行列-------*/
    if(rotate.x) {

        float sin = std::sin(rotate.x);
        float cos = std::cos(rotate.x);

        rotateMat[0].m[0][0] = 1;
        rotateMat[0].m[1][0] = 0;
        rotateMat[0].m[2][0] = 0;
        rotateMat[0].m[3][0] = 0;

        rotateMat[0].m[0][1] = 0;
        rotateMat[0].m[1][1] = cos;
        rotateMat[0].m[2][1] = -sin;
        rotateMat[0].m[3][1] = 0;

        rotateMat[0].m[0][2] = 0;
        rotateMat[0].m[1][2] = sin;
        rotateMat[0].m[2][2] = cos;
        rotateMat[0].m[3][2] = 0;

        rotateMat[0].m[0][3] = 0;
        rotateMat[0].m[1][3] = 0;
        rotateMat[0].m[2][3] = 0;
        rotateMat[0].m[3][3] = 1;
    } else {
        rotateMat[0] = IdentityMat4();
    }

    /*-------Y軸の回転行列-------*/
    if(rotate.y) {

        float sin = std::sin(rotate.y);
        float cos = std::cos(rotate.y);

        rotateMat[1].m[0][0] = cos;
        rotateMat[1].m[1][0] = 0;
        rotateMat[1].m[2][0] = sin;
        rotateMat[1].m[3][0] = 0;

        rotateMat[1].m[0][1] = 0;
        rotateMat[1].m[1][1] = 1;
        rotateMat[1].m[2][1] = 0;
        rotateMat[1].m[3][1] = 0;

        rotateMat[1].m[0][2] = -sin;
        rotateMat[1].m[1][2] = 0;
        rotateMat[1].m[2][2] = cos;
        rotateMat[1].m[3][2] = 0;

        rotateMat[1].m[0][3] = 0;
        rotateMat[1].m[1][3] = 0;
        rotateMat[1].m[2][3] = 0;
        rotateMat[1].m[3][3] = 1;
    } else {
        rotateMat[1] = IdentityMat4();
    }

    /*-------Z軸の回転行列-------*/
    if(rotate.z) {

        float sin = std::sin(rotate.z);
        float cos = std::cos(rotate.z);

        rotateMat[2].m[0][0] = cos;
        rotateMat[2].m[1][0] = -sin;
        rotateMat[2].m[2][0] = 0;
        rotateMat[2].m[3][0] = 0;

        rotateMat[2].m[0][1] = sin;
        rotateMat[2].m[1][1] = cos;
        rotateMat[2].m[2][1] = 0;
        rotateMat[2].m[3][1] = 0;

        rotateMat[2].m[0][2] = 0;
        rotateMat[2].m[1][2] = 0;
        rotateMat[2].m[2][2] = 1;
        rotateMat[2].m[3][2] = 0;

        rotateMat[2].m[0][3] = 0;
        rotateMat[2].m[1][3] = 0;
        rotateMat[2].m[2][3] = 0;
        rotateMat[2].m[3][3] = 1;
    } else {
        rotateMat[2] = IdentityMat4();
    }

    return Multiply(rotateMat[0], Multiply(rotateMat[1], rotateMat[2]));
}

/*------------------------- 平行移動行列を作る関数 --------------------------*/
Matrix3x3 TranslateMatrix(float tx, float ty) {
    Matrix3x3 matrix;
    matrix.m[0][0] = 1;  matrix.m[0][1] = 0;  matrix.m[0][2] = 0;
    matrix.m[1][0] = 0;  matrix.m[1][1] = 1;  matrix.m[1][2] = 0;
    matrix.m[2][0] = tx; matrix.m[2][1] = ty; matrix.m[2][2] = 1;
    return matrix;
}

Matrix3x3 TranslateMatrix(const Vector2& t) {
    Matrix3x3 matrix;
    matrix.m[0][0] = 1;   matrix.m[0][1] = 0;   matrix.m[0][2] = 0;
    matrix.m[1][0] = 0;   matrix.m[1][1] = 1;   matrix.m[1][2] = 0;
    matrix.m[2][0] = t.x; matrix.m[2][1] = t.y; matrix.m[2][2] = 1;
    return matrix;
}

Matrix4x4 TranslateMatrix(float tx, float ty, float tz) {
    Matrix4x4 matrix;
    matrix.m[0][0] = 1;  matrix.m[0][1] = 0;  matrix.m[0][2] = 0;  matrix.m[0][3] = 0;
    matrix.m[1][0] = 0;  matrix.m[1][1] = 1;  matrix.m[1][2] = 0;  matrix.m[1][3] = 0;
    matrix.m[2][0] = 0;  matrix.m[2][1] = 0;  matrix.m[2][2] = 1;  matrix.m[2][3] = 0;
    matrix.m[3][0] = tx; matrix.m[3][1] = ty; matrix.m[3][2] = tz; matrix.m[3][3] = 1;
    return matrix;
}

Matrix4x4 TranslateMatrix(const Vector3& t) {
    Matrix4x4 matrix;
    matrix.m[0][0] = 1;   matrix.m[0][1] = 0;   matrix.m[0][2] = 0;   matrix.m[0][3] = 0;
    matrix.m[1][0] = 0;   matrix.m[1][1] = 1;   matrix.m[1][2] = 0;   matrix.m[1][3] = 0;
    matrix.m[2][0] = 0;   matrix.m[2][1] = 0;   matrix.m[2][2] = 1;   matrix.m[2][3] = 0;
    matrix.m[3][0] = t.x; matrix.m[3][1] = t.y; matrix.m[3][2] = t.z; matrix.m[3][3] = 1;
    return matrix;
}


/*------------------------- アフィン行列を作る関数 --------------------------*/
Matrix3x3 AffineMatrix(Vector2 scale, float rotateTheta, Vector2 translate) {

    Matrix3x3 matrix;
    matrix.m[0][0] = scale.x * cosf(rotateTheta);
    matrix.m[0][1] = scale.x * sinf(rotateTheta);
    matrix.m[0][2] = 0;

    matrix.m[1][0] = scale.y * -sinf(rotateTheta);
    matrix.m[1][1] = scale.y * cosf(rotateTheta);
    matrix.m[1][2] = 0;

    matrix.m[2][0] = translate.x;
    matrix.m[2][1] = translate.y;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix4x4 AffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
    Matrix4x4 matrix(Multiply(ScaleMatrix(scale), RotateMatrix(rotate)));
    matrix.m[3][0] = translate.x;
    matrix.m[3][1] = translate.y;
    matrix.m[3][2] = translate.z;
    return matrix;
}

// 正則行列かどうか確認する関数--------------------------------------
// 3x3 行列の行列式を計算する関数
float Determinant3x3(const Matrix3x3& mat) {
    return
        (mat.m[0][0] * mat.m[1][1] * mat.m[2][2]) +
        (mat.m[0][1] * mat.m[1][2] * mat.m[2][0]) +
        (mat.m[0][2] * mat.m[1][0] * mat.m[2][1]) -
        (mat.m[0][2] * mat.m[1][1] * mat.m[2][0]) -
        (mat.m[0][1] * mat.m[1][0] * mat.m[2][2]) -
        (mat.m[0][0] * mat.m[1][2] * mat.m[2][1]);
}

// 4x4 行列の行列式を計算する関数
float Determinant4x4(const Matrix4x4& mat) {
    float det = 0.0f;
    for(int i = 0; i < 4; i++) {

        Matrix3x3 minor;

        for(int j = 0; j < 3; j++) {
            int k = 0;
            for(int l = 0; l < 4; l++) {
                if(l != i) {
                    minor.m[j][k++] = mat.m[j + 1][l];
                }
            }
        }

        float subDet = Determinant3x3(minor);
        det += (i % 2 == 0 ? 1.0f : -1.0f) * mat.m[0][i] * subDet;
    }
    return det;
}

//逆行列を求める関数
Matrix2x2 InverseMatrix(const Matrix2x2& matrix) {

    float det = (matrix.m[0][0] * matrix.m[1][1]) - (matrix.m[0][1] * matrix.m[1][0]);
    assert(det != 0);


    Matrix2x2 result;
    result.m[0][0] = matrix.m[1][1];
    result.m[0][1] = -matrix.m[0][1];

    result.m[1][0] = -matrix.m[1][0];
    result.m[1][1] = matrix.m[0][0];

    return Devide(result, det);
};

Matrix3x3 InverseMatrix(const Matrix3x3& matrix) {

    float det =
        (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2]) +
        (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0]) +
        (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1]) -
        (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0]) -
        (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2]) -
        (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]);

    assert(det != 0);

    Matrix3x3 result;
    result.m[0][0] = matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1];
    result.m[0][1] = -(matrix.m[0][1] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][1]);
    result.m[0][2] = matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1];

    result.m[1][0] = -(matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]);
    result.m[1][1] = matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0];
    result.m[1][2] = -(matrix.m[0][0] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][0]);

    result.m[2][0] = matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0];
    result.m[2][1] = -(matrix.m[0][0] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][0]);
    result.m[2][2] = matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0];

    return Devide(result, det);
}

Matrix4x4 InverseMatrix(const Matrix4x4& matrix) {

    Matrix4x4 inv;
    float sweep[4][8];

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            // sweepの左側に逆行列を求める行列をセット
            sweep[i][j] = matrix.m[i][j];

            // sweepの右半分は単位行列で初期化
            sweep[i][4 + j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // 左半分が単位行列になるまで繰り返す (右半分に逆行列が求められる)
    for(int col = 0; col < 4; col++) {

        /*------------------------------------------------------*/
        /*				       	ソート、除外						*/
        /*------------------------------------------------------*/

        /* 最大の絶対値を注目対角成分の絶対値と仮定 */
        float max = fabs(sweep[col][col]);
        int maxIdx = col;

        // 今見ている対角成分より大きい絶対値を持つ要素がその列のその行より後にあるか探す
        for(int row = col + 1; row < 4; row++) {
            if(fabs(sweep[row][col]) > max) {
                max = fabs(sweep[row][col]);
                maxIdx = row;
            }
        }

        // 最大値が0の場合、逆行列は求められない
        assert(fabs(sweep[maxIdx][col]) > 0);

        // 見つかった場合、その要素が見つかった行と今の行の要素を入れ替える
        if(col != maxIdx) {
            for(int col2 = 0; col2 < 8; col2++) {
                std::swap(sweep[maxIdx][col2], sweep[col][col2]);
            }
        }

        /*------------------------------------------------------*/
        /*				       	逆行列の計算						*/
        /*------------------------------------------------------*/

        /*--------- 今見ている列の対角成分を1にする ---------*/

        // 対角成分 sweep[col][col]に掛けると1になる値を求める
        float x = 1.0f / sweep[col][col];

        for(int col2 = 0; col2 < 8; col2++) {
            // この計算でsweep[col][col]が1になる 
            // (対角成分以外にもその行すべての要素に掛ける。)
            sweep[col][col2] *= x;
        }

        /*------- 今見ている列の対角成分以外を0にする -------*/
        for(int row = 0; row < 4; row++) {

            if(row == col) { continue; }// 対角成分はそのまま

            // 対角成分のある行以外に掛ける値を求める
            x = -sweep[row][col];

            for(int col2 = 0; col2 < 8; col2++) {
                // 対角成分を1にした行をa倍して足していく
                // すると対角成分以外のsweep[row][col]が0になる ( 自分に対して 1 x -自分 を足しているため。)
                sweep[row][col2] += sweep[col][col2] * x;
            }
        }
    }

    // sweepの右半分がmatrixの逆行列
    for(int row = 0; row < 4; row++) {
        for(int col = 0; col < 4; col++) {
            inv.m[row][col] = sweep[row][4 + col];
        }
    }

    return inv;
};

//転置行列を求める関数
Matrix2x2 Transpose(const Matrix2x2& matrix) {

    Matrix2x2 result;
    result.m[0][0] = matrix.m[0][0];
    result.m[0][1] = matrix.m[1][0];

    result.m[1][0] = matrix.m[0][1];
    result.m[1][1] = matrix.m[1][1];

    return result;
};

Matrix3x3 Transpose(const Matrix3x3& matrix) {

    Matrix3x3 result;
    result.m[0][0] = matrix.m[0][0];
    result.m[0][1] = matrix.m[1][0];
    result.m[0][2] = matrix.m[2][0];

    result.m[1][0] = matrix.m[0][1];
    result.m[1][1] = matrix.m[1][1];
    result.m[1][2] = matrix.m[2][1];

    result.m[2][0] = matrix.m[0][2];
    result.m[2][1] = matrix.m[1][2];
    result.m[2][2] = matrix.m[2][2];

    return result;
}

Matrix4x4 Transpose(const Matrix4x4& matrix) {
    Matrix4x4 result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = matrix.m[j][i];
        }
    }
    return result;
}


//正射影行列を求める関数
Matrix3x3 OrthoMatrix(float left, float right, float top, float bottom) {

    Matrix3x3 result;

    result.m[0][0] = 2.0f / (right - left);
    result.m[0][1] = 0;
    result.m[0][2] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[1][2] = 0;

    result.m[2][0] = (left + right) / (left - right);
    result.m[2][1] = (top + bottom) / (bottom - top);
    result.m[2][2] = 1;

    return result;
}

Matrix4x4 OrthoMatrix(float left, float right, float top, float bottom, float znear, float zfar) {

    Matrix4x4 result;

    result.m[0][0] = 2.0f / (right - left);
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1.0f / (zfar - znear);
    result.m[2][3] = 0;

    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = znear / (znear - zfar);
    result.m[3][3] = 1;

    return result;
}

float AspectRatio(float windowWidth, float windowHeight) {
    return windowWidth / windowHeight;
}

Matrix4x4 PerspectiveMatrix(float fovY, float aspectRatio, float znear, float zfar) {

    Matrix4x4 result;

    result.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = zfar / (zfar - znear);
    result.m[2][3] = 1;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = (-znear * zfar) / (zfar - znear);
    result.m[3][3] = 0;

    return result;
}

//ビューポート変換行列を求める関数
Matrix3x3 ViewportMatrix(const Vector2& size, const Vector2& LeftTop) {

    Matrix3x3 result;

    result.m[0][0] = size.x * 0.5f;
    result.m[0][1] = 0;
    result.m[0][2] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = -(size.y * 0.5f);
    result.m[1][2] = 0;

    result.m[2][0] = LeftTop.x + (size.x * 0.5f);
    result.m[2][1] = LeftTop.y + (size.y * 0.5f);
    result.m[2][2] = 1;

    return result;
}

Matrix4x4 ViewportMatrix(const Vector2& size, const Vector2& LeftTop, float minDepth, float maxDepth) {

    Matrix4x4 result;

    result.m[0][0] = size.x * 0.5f;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = -(size.y * 0.5f);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = maxDepth - minDepth;
    result.m[2][3] = 0;

    result.m[3][0] = LeftTop.x + (size.x * 0.5f);
    result.m[3][1] = LeftTop.y + (size.y * 0.5f);
    result.m[3][2] = minDepth;
    result.m[3][3] = 1;

    return result;
}
;


//レンダリングパイプライン作る関数
Matrix3x3 WvpVpMatrix(
    Vector2 playerPos, Vector2 playerScale, float playerAngle,
    Vector2 cameraPos, Vector2 cameraScale, float cameraAngle,
    Vector2 cameraRange, Vector2 leftTop,
    Vector2 screenSize
) {

    Matrix3x3 worldMatrix;
    Matrix3x3 cameraMatrix;
    Matrix3x3 viewMatrix;
    Matrix3x3 orthoMatrix;
    Matrix3x3 viewPortMatrix;
    Matrix3x3 result;

    worldMatrix = AffineMatrix(playerScale, playerAngle, playerPos);//プレイヤーのワールド行列
    cameraMatrix = AffineMatrix(cameraScale, cameraAngle, cameraPos);
    viewMatrix = InverseMatrix(cameraMatrix);//カメラの逆行列
    orthoMatrix = OrthoMatrix(//正射影行列
        -(cameraRange.x * 0.5f),
        cameraRange.x * 0.5f,
        -(cameraRange.y * 0.5f),
        cameraRange.y * 0.5f
    );
    viewPortMatrix = ViewportMatrix(//ビューポート行列
        screenSize, leftTop
    );

    //行列の合成
    result = Multiply(worldMatrix, viewMatrix);
    result = Multiply(result, orthoMatrix);
    result = Multiply(result, viewPortMatrix);

    return result;
}