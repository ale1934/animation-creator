#include "frame.hpp"
#include "settings.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <string>

// Constructor sets resolution of the frame to given by main()
Frame::Frame(Vector2 res) {
  canvas = LoadRenderTexture(res.x, res.y);
  resolution = res;

  BeginTextureMode(canvas);
  ClearBackground(RAYWHITE);
  EndTextureMode();

  saveState();
}

// Helper function to save the current canvas into a image snapshot
void Frame::saveState() {
  // A new stroke invalidates the redo history.
  for (Image &img : redoStates) {
    UnloadImage(img);
  }
  redoStates.clear();

  Image snapshot = LoadImageFromTexture(canvas.texture);

  if (states.size() >= MAX_STATES) {
    UnloadImage(states.front());
    states.pop_front();
  }

  states.push_back(snapshot);
}

// Handles Mouse Input (AKA draws on the frame)
void Frame::handleMouse(bool canDraw) {
  Vector2 mouse = GetMousePosition();

  bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

  // Start panning with Ctrl + left-click
  if (ctrlDown && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isPanning = true;
  }

  // Stop panning when the mouse button is released
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isPanning = false;
  }

  // Move the canvas with the mouse
  if (isPanning) {
    Vector2 delta = GetMouseDelta();

    g_settings.canvasOffset.x += delta.x;
    g_settings.canvasOffset.y += delta.y;
  }

  // Zoom toward the mouse cursor
  float wheel = GetMouseWheelMove();

  if (wheel != 0.0f) {
    float oldZoom = g_settings.zoom;

    g_settings.zoom *= powf(1.1f, wheel);
    g_settings.zoom =
        std::clamp(g_settings.zoom, g_settings.minZoom, g_settings.maxZoom);

    // Keep the point under the cursor stationary
    g_settings.canvasOffset.x =
        mouse.x -
        (mouse.x - g_settings.canvasOffset.x) * (g_settings.zoom / oldZoom);

    g_settings.canvasOffset.y =
        mouse.y -
        (mouse.y - g_settings.canvasOffset.y) * (g_settings.zoom / oldZoom);
  }

  // Convert screen coordinates to canvas coordinates
  mousePos = {(mouse.x - g_settings.canvasOffset.x) / g_settings.zoom,
              (mouse.y - g_settings.canvasOffset.y) / g_settings.zoom};

  leftButtonDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  rightButtonDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

  bool isDrawing =
      (leftButtonDown || rightButtonDown) && !isPanning && !ctrlDown;

  if (canDraw && isDrawing && !isPanning && !ctrlDown) {
    activeColor = rightButtonDown ? g_settings.palette[g_settings.bgColor]
                                  : g_settings.palette[g_settings.drawColor];

    // Only draw a segment if we were already drawing last frame.
    if (wasDrawing) {

      BeginTextureMode(canvas);
      DrawCircleV(prevMousePos, g_settings.lineThickness / 2.0f, activeColor);
      DrawCircleV(mousePos, g_settings.lineThickness / 2.0f, activeColor);
      DrawLineEx(prevMousePos, mousePos, g_settings.lineThickness, activeColor);
      EndTextureMode();
    } else {

      BeginTextureMode(canvas);
      DrawCircleV(mousePos, g_settings.lineThickness / 2.0f, activeColor);
      EndTextureMode();
    }
  }

  if (IsKeyPressed(KEY_C)) {
    BeginTextureMode(canvas);
    ClearBackground(RAYWHITE);
    EndTextureMode();
    wasDrawing = true;
  }

  // Save state if something changed and is not currently drawing
  if (wasDrawing && !isDrawing) {
    saveState();
  }

  // Update wasDrawing
  wasDrawing = isDrawing;

  // Undo Function -- if there is a past state then move current to
  // redo buffer and switch to that past state then remove it from states
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
    if (states.size() > 1) {
      redoStates.push_back(states.back());
      states.pop_back();

      UpdateTexture(canvas.texture, states.back().data);
    }
  }

  // Redo Function -- if there is a forward state then move current to
  // past buffer and switch to that forward state then remove it from
  // redoStates
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y)) {
    if (!redoStates.empty()) {
      states.push_back(redoStates.back());
      redoStates.pop_back();

      UpdateTexture(canvas.texture, states.back().data);
    }
  }

  // Change brush size with '+' or '-'
  if (IsKeyPressed(KEY_EQUAL)) {
    g_settings.lineThickness += 2.0f;
  } else if (IsKeyPressed(KEY_MINUS)) {
    g_settings.lineThickness -= 2.0f;
  }

  prevMousePos = mousePos;
}

// Draws Frame Texture onto Screen
void Frame::renderFrame(int frameIndex) {
  Rectangle source = {0, 0, (float)canvas.texture.width,
                      (float)-canvas.texture.height};

  Rectangle dest = {g_settings.canvasOffset.x, g_settings.canvasOffset.y,
                    resolution.x * g_settings.zoom,
                    resolution.y * g_settings.zoom};

  DrawTexturePro(canvas.texture, source, dest, {0, 0}, 0.0f, WHITE);

  // Brush preview remains centered on the screen cursor
  DrawCircleLinesEx(GetMousePosition(),
                    g_settings.lineThickness * g_settings.zoom / 2.0f, 2.0f,
                    (Color){127, 127, 127, 255});

  // Draw Frame index at the bottom
  std::string frameId = "Frame: " + std::to_string(frameIndex);
  DrawTextEx(g_settings.UiFont, frameId.c_str(),
             {(float)canvas.texture.width / 2 -
                  (float)MeasureText(frameId.c_str(), 32) / 2,
              (float)canvas.texture.height - 40},
             32, 0.0f, BLUE);
}

void Frame::renderOnion(Shader &shader, int tintLoc, Color tint) {
  float c[4] = {tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f,
                tint.a / 255.0f};

  SetShaderValue(shader, tintLoc, c, SHADER_UNIFORM_VEC4);

  Rectangle source = {0, 0, (float)canvas.texture.width,
                      (float)-canvas.texture.height};

  Rectangle dest = {g_settings.canvasOffset.x, g_settings.canvasOffset.y,
                    resolution.x * g_settings.zoom,
                    resolution.y * g_settings.zoom};

  BeginShaderMode(shader);

  DrawTexturePro(canvas.texture, source, dest, {0, 0}, 0.0f, WHITE);

  EndShaderMode();
}

void Frame::setImage(Image image) {
  ImageFlipVertical(&image);
  ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

  // Upload pixels into the existing texture so the render target stays valid
  UpdateTexture(canvas.texture, image.data);

  saveState();

  UnloadImage(image);
}

Image Frame::getImage() {
  Image img = LoadImageFromTexture(canvas.texture);
  ImageFlipVertical(&img);
  ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  return img;
}
