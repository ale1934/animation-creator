#pragma once

#include <raylib.h>

struct Settings {
  Color drawColor = BLACK;
  Color bgColor = RAYWHITE;
  float lineThickness = 4.0f;
  bool needsInput = false;

  Vector2 canvasOffset = {0.0f, 0.0f};

  float zoom = 1.0f;
  float minZoom = 0.1f;
  float maxZoom = 8.0f;

  Color UIBackground = GetColor(0xe6e6faff);
  Color UIBackgroundBorder = GetColor(0x555555ff);
  Color UITextColor = DARKBLUE;
};

// Declared here, defined once in settings.cpp
extern Settings g_settings;
