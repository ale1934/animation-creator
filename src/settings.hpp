#pragma once

#include <raylib.h>

struct Settings {
  Color drawColor = BLACK;
  Color bgColor = RAYWHITE;
  float lineThickness = 4.0f;
  bool needsInput = false;
};

// Declared here, defined once in settings.cpp
extern Settings g_settings;
