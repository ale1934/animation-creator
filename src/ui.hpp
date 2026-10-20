#pragma once

#include "animation.hpp"
#include "widgets.hpp"
#include <raylib.h>

class UI {
private:
  Vector2 dimensions;

  bool needsInput = false;
  bool showTools = false;

  Animation *currentAnimation;

  Font UiFont;

  void panel();
  void toolBar();

  ColorWheelState wheel;
  Color col1 = BLACK;
  Color col2 = RED;
  std::vector<Color> currentPallete = {col1, col2};

  Rectangle toolBarRect;

public:
  UI(Vector2 dim, Animation &anim);

  void update();
  void render();

  bool getNeedInput() const;
};
