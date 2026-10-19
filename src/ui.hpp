#pragma once

#include "animation.hpp"
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

  Rectangle toolBarRect;
  Color toolBarColor = RAYWHITE;
  Color toolBarBorderColor = GetColor(0x000103ff);

public:
  UI(Vector2 dim, Animation &anim);

  void update();
  void render();

  bool getNeedInput() const;
};
