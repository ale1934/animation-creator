#include "frame.hpp"
#include <raylib.h>

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
  mousePos = GetMousePosition();

  leftButtonDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  rightButtonDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

  bool isDrawing = leftButtonDown || rightButtonDown;

  if (canDraw) {

    if (isDrawing) {
      activeColor = rightButtonDown ? bgColor : drawColor;

      // Draw the line onto the canvas
      BeginTextureMode(canvas);

      // Draw circles to make lines looks smoother
      DrawCircleV(prevMousePos, lineThickness / 2.0f, activeColor);
      DrawCircleV(mousePos, lineThickness / 2.0f, activeColor);
      DrawLineEx(prevMousePos, mousePos, lineThickness, activeColor);

      EndTextureMode();
    }
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
  // past buffer and switch to that forward state then remove it from redoStates
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y)) {
    if (!redoStates.empty()) {
      states.push_back(redoStates.back());
      redoStates.pop_back();

      UpdateTexture(canvas.texture, states.back().data);
    }
  }

  prevMousePos = GetMousePosition();
}

// Draws Frame Texture onto Screen
void Frame::renderFrame() {
  // Draw the render texture to the screen
  DrawTextureRec(canvas.texture,
                 Rectangle{0, 0, (float)canvas.texture.width,
                           (float)-canvas.texture.height},
                 Vector2{0, 0}, WHITE);

  // Draw the preview circle
  if (!leftButtonDown)
    DrawCircleLinesV(GetMousePosition(), lineThickness / 2.0f,
                     (Color){127, 127, 127, 127});
}

void Frame::renderOnion(Shader &shader, int tintLoc, Color tint) {
  float c[4] = {tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f,
                tint.a / 255.0f};
  SetShaderValue(shader, tintLoc, c, SHADER_UNIFORM_VEC4);

  BeginShaderMode(shader);
  DrawTextureRec(canvas.texture,
                 Rectangle{0, 0, (float)canvas.texture.width,
                           (float)-canvas.texture.height},
                 Vector2{0, 0}, WHITE);
  EndShaderMode();
}

Image Frame::getImage() {
  Image img = LoadImageFromTexture(canvas.texture);
  ImageFlipVertical(&img);
  ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  return img;
}
