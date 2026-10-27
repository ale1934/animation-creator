#include "animation.hpp"
#include "settings.hpp"
#include <raylib.h>

void DrawTextCenteredEx(Font font, std::string str, int fontSize, Color color,
                        float yOffset) {
  Vector2 textSize = MeasureTextEx(font, str.c_str(), fontSize, 0.0f);
  DrawTextEx(font, str.c_str(),
             {GetScreenWidth() / 2.0f - textSize.x / 2,
              GetScreenHeight() / 2.0f - textSize.y / 2 - yOffset},
             fontSize, 0.0f, color);
}

int main(int argc, char *argv[]) {
  float screenWidth = 960;
  float screenHeight = 640;
  int fps = 4;

  bool hasProject = false;
  bool creatingProject = false;
  bool inPause = false;
  bool showHelp = false;

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

  std::string selectedProject;

  Vector2 selectedResolution = {screenWidth, screenHeight};
  int selectedFps = 4;

  bool selectingResX = true;
  bool selectingResY = false;
  bool selectingFps = false;

  std::string resumeText = "Resume";
  std::string helpText = "Keybinds";

  Vector2 resumeSize =
      MeasureTextEx(g_settings.UiFont, resumeText.c_str(), fontSize, 0.0f);
  Vector2 helpSize =
      MeasureTextEx(g_settings.UiFont, helpText.c_str(), fontSize, 0.0f);

  bool selectingResume = true;
  bool selectingHelp = false;

  bool selectingBack = true;

  SetExitKey(KEY_NULL);

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_ESCAPE)) {
      inPause = !inPause;
    }

    if (!hasProject && !creatingProject && !inPause) {
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

        myAnimation =
            Animation({(float)anim.width, (float)anim.height}, fps, 1);

        SetWindowSize(anim.width, anim.height);

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

      DrawTextCenteredEx(g_settings.UiFont, dragText, fontSize, BLACK, 20);

      DrawTextCenteredEx(g_settings.UiFont, bottomText, fontSize, BLACK, -10);

      EndDrawing();

    } else if (creatingProject && !inPause) {

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
        screenWidth = selectedResolution.x;
        screenHeight = selectedResolution.y;
        creatingProject = false;
        hasProject = true;
      }

      BeginDrawing();
      ClearBackground(RAYWHITE);

      DrawTextEx(g_settings.UiFont, resBaseText.c_str(),
                 {screenWidth / 2.0f - resSize.x / 2.0f,
                  screenHeight / 2.0f - resSize.y / 2 - 20},
                 g_settings.UiFont.baseSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, resTextX.c_str(),
                 {screenWidth / 2.0f - resSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont, resBaseText.c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f - resSize.y / 2 - 20},
                 fontSize, 0.0f, selectingResX ? BLUE : BLACK);

      DrawTextEx(
          g_settings.UiFont, "x",
          {screenWidth / 2.0f - resSize.x / 2.0f +
               MeasureTextEx(g_settings.UiFont,
                             (resBaseText + resTextX).c_str(), fontSize, 0.0f)
                   .x,
           screenHeight / 2.0f - resSize.y / 2 - 20},
          fontSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, resTextY.c_str(),
                 {screenWidth / 2.0f - resSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont,
                                    (resBaseText + resTextX + "x").c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f - resSize.y / 2 - 20},
                 fontSize, 0.0f, selectingResY ? BLUE : BLACK);

      DrawTextEx(g_settings.UiFont, fpsBaseText.c_str(),
                 {screenWidth / 2.0f - fpsSize.x / 2.0f,
                  screenHeight / 2.0f - fpsSize.y / 2 + 10},
                 g_settings.UiFont.baseSize, 0.0f, BLACK);

      DrawTextEx(g_settings.UiFont, fpsText.c_str(),
                 {screenWidth / 2.0f - fpsSize.x / 2.0f +
                      MeasureTextEx(g_settings.UiFont, fpsBaseText.c_str(),
                                    fontSize, 0.0f)
                          .x,
                  screenHeight / 2.0f - fpsSize.y / 2 + 10},
                 fontSize, 0.0f, selectingFps ? BLUE : BLACK);

      EndDrawing();
    } else if (inPause) {

      if (IsKeyPressed(KEY_DOWN)) {
        if (selectingResume && !showHelp) {
          selectingResume = false;
          selectingHelp = true;
        }
      }

      if (IsKeyPressed(KEY_UP)) {

        if (selectingHelp && !showHelp) {
          selectingHelp = false;
          selectingResume = true;
        }
      }

      if (IsKeyPressed(KEY_ENTER)) {
        if (!showHelp) {
          if (selectingResume) {
            inPause = false;
          } else {
            showHelp = true;
          }
        } else {
          showHelp = false;
        }
      }

      BeginDrawing();

      ClearBackground(RAYWHITE);

      if (!showHelp) {
        DrawTextCenteredEx(g_settings.UiFont, "Paused", fontSize, BLACK, 250);

        DrawTextEx(g_settings.UiFont, resumeText.c_str(),
                   {GetScreenWidth() / 2.0f - resumeSize.x / 2,
                    GetScreenHeight() / 2.0f - resumeSize.y / 2 - 20},
                   fontSize, 0.0f, selectingResume ? BLUE : BLACK);

        DrawTextEx(g_settings.UiFont, helpText.c_str(),
                   {GetScreenWidth() / 2.0f - helpSize.x / 2,
                    GetScreenHeight() / 2.0f - helpSize.y / 2 + 10},
                   fontSize, 0.0f, selectingHelp ? BLUE : BLACK);
      } else {
        DrawTextCenteredEx(g_settings.UiFont, "Back", fontSize,
                           selectingBack ? BLUE : BLACK, 0);
      }

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
