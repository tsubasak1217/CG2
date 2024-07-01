#pragma once
#include <stdint.h>
#include "Vector2.h"

static const char* kWindowTitle = "CG2_Triangle";

static const uint32_t kWindowSizeX = 1280;
static const uint32_t kWindowSizeY = 720;
static const Vector2 kWindowSize = { float(kWindowSizeX),float(kWindowSizeY) };
static const Vector2 kWindowCenter = kWindowSize * 0.5f;