#pragma once

#include <raylib.h>

struct Settings {
  Color palette[7] = {BLACK, RED, BLUE, GREEN, PURPLE, YELLOW, RAYWHITE};

  int drawColor = 0;
  int bgColor = 6;
  float lineThickness = 8.0f;

  Font UiFont;

  Vector2 canvasOffset = {0.0f, 0.0f};

  float zoom = 1.0f;
  float minZoom = 0.1f;
  float maxZoom = 8.0f;
};

// Declared here, defined once in settings.cpp
extern Settings g_settings;
