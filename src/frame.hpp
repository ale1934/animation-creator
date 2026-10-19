#pragma once

#include "settings.hpp"
#include <deque>
#include <raylib.h>

#define MAX_STATES 20

class Frame {
private:
  RenderTexture canvas;
  Vector2 resolution = {0, 0};
  Vector2 mousePos = {0, 0};
  Vector2 prevMousePos = {0, 0};

  int index;

  bool leftButtonDown = false;
  bool rightButtonDown = false;

  // Handles image states to allow undo/redo
  std::deque<Image> states;     // Can handle 20 states
  std::deque<Image> redoStates; // Can handle 20 states

  bool wasDrawing = false;
  bool isDrawing = false;

  float lineThickness = 8.0f;

  Color activeColor = g_settings.drawColor;

  void saveState(); // Saves image state

public:
  Frame(Vector2 res);
  void handleMouse(bool canDraw);
  void renderFrame();
  Image getImage();

  void renderOnion(Shader &shader, int tintLoc, Color tint);
  void clearMousePos();
};
