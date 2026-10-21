#include "animation.hpp"
#include "settings.hpp"
#include <raylib.h>

int main(int argc, char *argv[]) {
  float screenWidth = 960;
  float screenHeight = 640;
  int fps = 4;

  if (argc > 1) {
    std::string resolution = argv[1];

    size_t separator = resolution.find(':');

    if (separator != std::string::npos) {
      screenWidth = std::stoi(resolution.substr(0, separator));
      screenHeight = std::stoi(resolution.substr(separator + 1));
    }

    if (argc > 2) {
      fps = std::stoi(argv[1]);
    }
  }

  InitWindow(screenWidth, screenHeight, "Animation Creator");

  g_settings.UiFont =
      LoadFont("resources/CascadiaMono/CaskaydiaMonoNerdFont-Regular.ttf");

  Animation myAnimation = Animation({screenWidth, screenHeight}, fps, 1);

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
