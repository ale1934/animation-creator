#include "animation.hpp"
#include "settings.hpp"
#include <raylib.h>

int main() {
  const int screenWidth = 960;
  const int screenHeight = 640;
  InitWindow(screenWidth, screenHeight, "Animation Creator");

  g_settings.UiFont =
      LoadFont("resources/CascadiaMono/CaskaydiaMonoNerdFont-Regular.ttf");

  Animation myAnimation = Animation({screenWidth, screenHeight}, 4, 1);

  SetTargetFPS(165);

  HideCursor();

  while (!WindowShouldClose()) {

    myAnimation.update();

    BeginDrawing();
    ClearBackground(Color{35, 38, 45, 255});

    myAnimation.render();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
