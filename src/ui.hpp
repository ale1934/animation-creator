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
  Color col2 = GRAY;
  Color col3 = BLUE;
  Color col4 = RED;
  Color col5 = GREEN;
  std::vector<Color> currentPallete = {col1, col2, col3, col4, col5};

  Rectangle toolBarRect;
  Color toolBarColor = RAYWHITE;
  Color toolBarBorderColor = GetColor(0x000103ff);

public:
  UI(Vector2 dim, Animation &anim);

  void update();
  void render();

  bool getNeedInput() const;
};
