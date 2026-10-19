#include "animation.hpp"
#include "ui.hpp"
#include <raylib.h>

int main() {
  const int screenWidth = 960;
  const int screenHeight = 640;
  InitWindow(screenWidth, screenHeight, "Animation Creator");

  Animation myAnimation = Animation({screenWidth, screenHeight}, 4, 1);
  UI myUI = UI({screenWidth, screenHeight}, myAnimation);

  SetTargetFPS(165);

  while (!WindowShouldClose()) {
    myUI.update();

    if (!myUI.getNeedInput()) {
      HideCursor();
      myAnimation.update();
    } else
      ShowCursor();

    BeginDrawing();
    ClearBackground(Color{35, 38, 45, 255});

    myAnimation.render();
    myUI.render();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
