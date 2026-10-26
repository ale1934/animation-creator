#include "animation.hpp"
#include "settings.hpp"
#include <raylib.h>

int main(int argc, char *argv[]) {
  float screenWidth = 960;
  float screenHeight = 640;
  int fps = 4;

  bool hasProject = false;

  InitWindow(screenWidth, screenHeight, "Animation Creator");

  g_settings.UiFont =
      LoadFont("resources/CascadiaMono/CaskaydiaMonoNerdFont-Regular.ttf");

  Animation myAnimation = Animation({screenWidth, screenHeight}, fps, 1);

  SetTargetFPS(165);

  char dragText[] = "drag and drop project here!";
  char bottomText[] = "or press enter to create new project";

  const float fontSize = g_settings.UiFont.baseSize;
  const float smallFontSize = fontSize / 2.0f;

  Vector2 dragSize = MeasureTextEx(g_settings.UiFont, dragText, fontSize, 0.0f);

  Vector2 bottomSize =
      MeasureTextEx(g_settings.UiFont, bottomText, smallFontSize, 0.0f);

  std::string selectedProject;

  while (!WindowShouldClose()) {

    if (!hasProject) {
      FilePathList filepath = LoadDroppedFiles();

      if (IsKeyDown(KEY_ENTER)) {
        HideCursor();

        hasProject = true;
      }

      if (filepath.count > 0) {
        selectedProject = filepath.paths[filepath.count - 1];

        TraceLog(LOG_INFO, "Selected Project %s", selectedProject.c_str());

        TraceLog(LOG_INFO, "Loading Anim %s", selectedProject.c_str());

        int frames = 0;
        Image anim = LoadImageAnim(selectedProject.c_str(), &frames);

        for (int i = 0; i < frames; i++) {
          unsigned int nextFrameDataOffset = anim.width * anim.height * 4 * i;
          Texture2D temp = LoadTextureFromImage(anim);
          UpdateTexture(temp, (unsigned char *)anim.data + nextFrameDataOffset);

          myAnimation.setFrame(i, temp);
        }

        UnloadDroppedFiles(filepath);

        HideCursor();
        hasProject = true;
      }

      BeginDrawing();

      ClearBackground(GetColor(0x333333FF));

      DrawTextEx(
          g_settings.UiFont, dragText,
          {screenWidth / 2.0f - dragSize.x / 2.0f, screenHeight / 2.0f - 20},
          fontSize, 0.0f, RAYWHITE);

      DrawTextEx(g_settings.UiFont, bottomText,
                 {screenWidth / 2.0f - bottomSize.x / 2.0f,
                  screenHeight / 2.0f + 20.0f},
                 smallFontSize, 0.0f, RAYWHITE);

      EndDrawing();

    } else {
      myAnimation.update();

      BeginDrawing();
      ClearBackground(Color{35, 38, 45, 255});

      myAnimation.render();

      EndDrawing();
    }
  }

  CloseWindow();

  return 0;
}
