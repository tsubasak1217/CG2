#pragma once
struct Matrix3x3 final {
    float m[3][3];

    Matrix3x3() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                m[i][j] = 0.0f;
            }
        }
    }

    Matrix3x3(const Matrix3x3& init) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                m[i][j] = init.m[i][j];
            }
        }
    }

    Matrix3x3(float aa, float ab, float ac, float ba, float bb, float bc, float ca, float cb, float cc) {
        m[0][0] = aa;
        m[0][1] = ab;
        m[0][2] = ac;
        m[1][0] = ba;
        m[1][1] = bb;
        m[1][2] = bc;
        m[2][0] = ca;
        m[2][1] = cb;
        m[2][2] = cc;
    }
};
