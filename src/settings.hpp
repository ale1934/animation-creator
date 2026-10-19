#pragma once

#include <raylib.h>

struct Settings {
  Color drawColor = BLACK;
  Color bgColor = RAYWHITE;
  float lineThickness = 4.0f;
  bool needsInput = false;

  Color UIBackground = GetColor(0xe6e6faff);
  Color UIBackgroundBorder = GetColor(0x555555ff);
  Color UITextColor = BLACK;
};

// Declared here, defined once in settings.cpp
extern Settings g_settings;
