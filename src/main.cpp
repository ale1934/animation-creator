#include "animation.hpp"
#include "settings.hpp"
#include <raylib.h>

int main(int argc, char *argv[]) {
  float screenWidth = 960;
  float screenHeight = 640;
  int fps = 4;

  bool hasProject = false;
  bool creatingProject = false;

  InitWindow(screenWidth, screenHeight, "Animation Creator");

  g_settings.UiFont =
      LoadFont("resources/CascadiaMono/CaskaydiaMonoNerdFont-Regular.ttf");

  Animation myAnimation = Animation({screenWidth, screenHeight}, fps, 1);

  SetTargetFPS(165);

  char dragText[] = "drag and drop project here!";
  char bottomText[] = "or press enter to create new project";
  std::string resBaseText = "Selected Resolution: ";
  std::string fpsBaseText = "Selected Fps: ";

  const float fontSize = g_settings.UiFont.baseSize;
  const float smallFontSize = fontSize / 2.0f;

  Vector2 dragSize = MeasureTextEx(g_settings.UiFont, dragText, fontSize, 0.0f);

  Vector2 bottomSize =
      MeasureTextEx(g_settings.UiFont, bottomText, smallFontSize, 0.0f);

  std::string selectedProject;

  Vector2 selectedResolution = {screenWidth, screenHeight};
  int selectedFps = 4;

  bool selectingResX = true;
  bool selectingResY = false;
  bool selectingFps = false;

  while (!WindowShouldClose()) {

    if (!hasProject && !creatingProject) {
      FilePathList filepath = LoadDroppedFiles();

      if (IsKeyDown(KEY_ENTER)) {
        HideCursor();
        creatingProject = true;
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

      ClearBackground(RAYWHITE);

      DrawTextEx(
          g_settings.UiFont, dragText,
          {screenWidth / 2.0f - dragSize.x / 2.0f, screenHeight / 2.0f - 20},
          fontSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, bottomText,
                 {screenWidth / 2.0f - bottomSize.x / 2.0f,
                  screenHeight / 2.0f + 20.0f},
                 smallFontSize, 0.0f, BLACK);

      EndDrawing();

    } else if (creatingProject) {

      std::string resTextX = TextFormat("%.0f", selectedResolution.x);

      std::string resTextY = TextFormat("%.0f", selectedResolution.y);

      std::string fpsText = TextFormat("%d", selectedFps);

      Vector2 resSize = MeasureTextEx(
          g_settings.UiFont, (resBaseText + resTextX + resTextY).c_str(),
          fontSize, 0.0f);

      Vector2 fpsSize = MeasureTextEx(
          g_settings.UiFont, (fpsBaseText + fpsText).c_str(), fontSize, 0.0f);

      if (IsKeyPressed(KEY_RIGHT) && !selectingFps) {
        if (selectingResX) {
          selectingResX = false;
          selectingResY = true;
        } else {
          selectingResY = false;
          selectingFps = true;
        }
      }

      if (IsKeyPressed(KEY_LEFT) && !selectingResX) {
        if (selectingResY) {
          selectingResY = false;
          selectingResX = true;
        } else {
          selectingFps = false;
          selectingResY = true;
        }
      }

      if (IsKeyPressed(KEY_UP)) {
        if (selectingResX) {
          selectedResolution.x += 40;
        } else if (selectingResY) {
          selectedResolution.y += 40;
        } else {
          selectedFps += 2;
        }
      }

      if (IsKeyPressed(KEY_DOWN)) {
        if (selectingResX) {
          selectedResolution.x -= 40;
        } else if (selectingResY) {
          selectedResolution.y -= 40;
        } else {
          selectedFps -= 2;
        }
      }

      if (IsKeyPressed(KEY_ENTER)) {
        myAnimation = Animation({selectedResolution.x, selectedResolution.y},
                                selectedFps, 1);
        SetWindowSize(selectedResolution.x, selectedResolution.y);
        creatingProject = false;
        hasProject = true;
      }

      BeginDrawing();
      ClearBackground(RAYWHITE);

      DrawTextEx(
          g_settings.UiFont, resBaseText.c_str(),
          {screenWidth / 2.0f - resSize.x / 2.0f, screenHeight / 2.0f - 30},
          g_settings.UiFont.baseSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, resTextX.c_str(),
                 {screenWidth / 2.0f - resSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont, resBaseText.c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f - 30},
                 fontSize, 0.0f, selectingResX ? BLUE : BLACK);

      DrawTextEx(
          g_settings.UiFont, "x",
          {screenWidth / 2.0f - resSize.x / 2.0f +
               MeasureTextEx(g_settings.UiFont,
                             (resBaseText + resTextX).c_str(), fontSize, 0.0f)
                   .x,
           screenHeight / 2.0f - 30},
          fontSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, resTextY.c_str(),
                 {screenWidth / 2.0f - resSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont,
                                    (resBaseText + resTextX + "x").c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f - 30},
                 fontSize, 0.0f, selectingResY ? BLUE : BLACK);

      DrawTextEx(
          g_settings.UiFont, fpsBaseText.c_str(),
          {screenWidth / 2.0f - fpsSize.x / 2.0f, screenHeight / 2.0f + 10},
          g_settings.UiFont.baseSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, fpsText.c_str(),
                 {screenWidth / 2.0f - fpsSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont, fpsBaseText.c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f + 10},
                 fontSize, 0.0f, selectingFps ? BLUE : BLACK);

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
