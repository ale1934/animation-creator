#include "widgets.hpp"
#include <math.h>
#include <raylib.h>

static void drawCenteredText(Font font, Rectangle rect, const char *text,
                             Color color, float fontSize,
                             float spacing = 1.0f) {
  Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);

  // Use the font's base height rather than the measured line height,
  // which includes extra leading that pushes the visible glyphs off-center.
  float visibleHeight = fontSize;

  Vector2 position = {
      rect.x + (rect.width - textSize.x) / 2.0f,
      rect.y + (rect.height - visibleHeight) / 2.0f,
  };

  position.x = round(position.x);
  position.y = round(position.y);

  DrawTextEx(font, text, position, fontSize, spacing, color);
}

bool Widgets::Panel(Rectangle dimensions, Color color, float roundness,
                    bool borderEnabled, Color borderColor,
                    float borderThickness) {
  DrawRectangleRounded(dimensions, roundness, SEGMENTS, color);
  if (borderEnabled)
    DrawRectangleRoundedLinesEx(dimensions, roundness, SEGMENTS,
                                borderThickness, borderColor);

  if (CheckCollisionPointRec(GetMousePosition(), dimensions)) {
    return true;
  }
  return false;
}

void Widgets::Label(Font UiFont, Rectangle dimensions, std::string label,
                    Color color, float roundness, bool borderEnabled,
                    Color borderColor, float borderThickness) {
  DrawRectangleRounded(dimensions, roundness, SEGMENTS, color);

  drawCenteredText(UiFont, dimensions, label.c_str(), BLACK, FONTSIZE);

  Vector2 center = {dimensions.x + dimensions.width / 2.0f,
                    dimensions.y + dimensions.height / 2.0f};

  if (borderEnabled)
    DrawRectangleRoundedLinesEx(dimensions, roundness, SEGMENTS,
                                borderThickness, borderColor);
}

bool Widgets::Button(Font UiFont, Rectangle dimensions, std::string label,
                     Color color, float roundness, bool borderEnabled,
                     Color borderColor, float borderThickness) {
  DrawRectangleRounded(dimensions, roundness, SEGMENTS, color);
  bool mouseHover = CheckCollisionPointRec(GetMousePosition(), dimensions);

  if (mouseHover) {
    drawCenteredText(UiFont, dimensions, label.c_str(), Fade(BLACK, 0.6f),
                     FONTSIZE);
  } else {
    drawCenteredText(UiFont, dimensions, label.c_str(), BLACK, FONTSIZE);
  }

  Vector2 center = {dimensions.x + dimensions.width / 2.0f,
                    dimensions.y + dimensions.height / 2.0f};

  if (borderEnabled)
    DrawRectangleRoundedLinesEx(dimensions, roundness, SEGMENTS,
                                borderThickness, borderColor);

  return mouseHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
