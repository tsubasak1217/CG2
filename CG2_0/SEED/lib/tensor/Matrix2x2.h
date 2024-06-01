#pragma once
struct Matrix2x2 final {
    float m[2][2];

    Matrix2x2() {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                m[i][j] = 0.0f;
            }
        }
    }

    Matrix2x2(const Matrix2x2& init) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                m[i][j] = init.m[i][j];
            }
        }
    }

    Matrix2x2(float aa, float ab, float ba, float bb) {
        m[0][0] = aa;
        m[0][1] = ab;
        m[1][0] = ba;
        m[1][1] = bb;
    }
};