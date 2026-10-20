#include "widgets.hpp"
#include "settings.hpp"
#include <algorithm>
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

  drawCenteredText(UiFont, dimensions, label.c_str(), g_settings.UITextColor,
                   FONTSIZE);

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
    drawCenteredText(UiFont, dimensions, label.c_str(),
                     Fade(g_settings.UITextColor, 0.6f), FONTSIZE);
  } else {
    drawCenteredText(UiFont, dimensions, label.c_str(), g_settings.UITextColor,
                     FONTSIZE);
  }

  Vector2 center = {dimensions.x + dimensions.width / 2.0f,
                    dimensions.y + dimensions.height / 2.0f};

  if (borderEnabled)
    DrawRectangleRoundedLinesEx(dimensions, roundness, SEGMENTS,
                                borderThickness, borderColor);

  return mouseHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Widgets::ColorEdit(Rectangle dimensions, Color &color,
                        std::vector<Color> &palette, ColorWheelState &wheel) {
  DrawRectangleRec(dimensions, g_settings.UIBackground);

  if (palette.empty()) {
    return;
  }

  float padding = 2.0f;
  float colSize = dimensions.width / palette.size();
  Vector2 mouse = GetMousePosition();

  for (size_t i = 0; i < palette.size(); i++) {
    Rectangle rect = {dimensions.x + padding + i * colSize,
                      dimensions.y + padding, colSize - padding,
                      dimensions.height - 2 * padding};

    DrawRectangleRounded(rect, 0.3f, 8, palette[i]);

    // Right-click a swatch to open its color wheel.
    if (CheckCollisionPointRec(mouse, rect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      wheel.open = true;
      wheel.selectedIndex = (int)i;

      // Place the wheel at the center of the screen
      wheel.center = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    }

    // Left-click a swatch to select it as color
    if (CheckCollisionPointRec(mouse, rect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      color = palette.at(i);
    }
  }
}

bool Widgets::ColorWheel(std::vector<Color> &palette, ColorWheelState &wheel) {
  if (!wheel.open || wheel.selectedIndex < 0 ||
      wheel.selectedIndex >= (int)palette.size()) {
    return false;
  }

  Vector2 mouse = GetMousePosition();

  // Close when clicking outside the wheel.
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
      CheckCollisionPointCircle(mouse, wheel.center, wheel.outerRadius + 12)) {
    wheel.open = false;
    return false;
  }

  // Draw a dark background behind the wheel.
  DrawCircleV(wheel.center, wheel.outerRadius + 6, Color{35, 35, 35, 245});

  // Draw the hue ring as colored triangular segments.
  const int segments = 120;
  const float inner = wheel.innerRadius;
  const float outer = wheel.outerRadius;

  for (int i = 0; i < segments; i++) {
    float a1 = (float)i / segments * 2.0f * PI - PI / 2.0f;
    float a2 = (float)(i + 1) / segments * 2.0f * PI - PI / 2.0f;

    Vector2 inner1 = {wheel.center.x + cosf(a1) * inner,
                      wheel.center.y + sinf(a1) * inner};
    Vector2 inner2 = {wheel.center.x + cosf(a2) * inner,
                      wheel.center.y + sinf(a2) * inner};
    Vector2 outer1 = {wheel.center.x + cosf(a1) * outer,
                      wheel.center.y + sinf(a1) * outer};
    Vector2 outer2 = {wheel.center.x + cosf(a2) * outer,
                      wheel.center.y + sinf(a2) * outer};

    Color hue = ColorFromHSV((float)i / segments * 360.0f, 1.0f, 1.0f);

    // Counter-clockwise winding so raylib doesn't cull the triangles.
    DrawTriangle(inner1, outer2, outer1, hue);
    DrawTriangle(inner1, inner2, outer2, hue);
  }

  // Show the currently selected color in the center.
  Color current = palette[wheel.selectedIndex];

  DrawCircleV(wheel.center, inner - 5, current);
  DrawCircleLinesV(wheel.center, inner - 5, RAYWHITE);

  // Update hue while clicking or dragging on the ring.
  float dx = mouse.x - wheel.center.x;
  float dy = mouse.y - wheel.center.y;
  float distance = sqrtf(dx * dx + dy * dy);

  if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
       IsMouseButtonDown(MOUSE_BUTTON_LEFT)) &&
      distance >= inner && distance <= outer) {

    float angle = atan2f(dy, dx) + PI / 2.0f;

    if (angle < 0.0f) {
      angle += 2.0f * PI;
    }

    float hue = angle / (2.0f * PI) * 360.0f;

    Vector3 hsv = ColorToHSV(current);

    float saturation = hsv.y;
    if (saturation < 0.01f) {
      saturation = 1.0f;
    }

    float value = hsv.z;
    if (value < 0.01f) {
      value = 1.0f;
    }

    palette[wheel.selectedIndex] = ColorFromHSV(hue, saturation, value);
  }

  // Left-click outside the wheel closes it or pressed enter
  if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && distance > outer + 12) ||
      IsKeyPressed(KEY_ENTER)) {
    wheel.open = false;
    g_settings.drawColor = palette.at(wheel.selectedIndex);
  }

  return wheel.open;
}

void Widgets::FloatSlider(Rectangle dimensions, Color sliderColor,
                          Color buttonColor, float &value, float minValue,
                          float maxValue) {
  DrawLineEx({dimensions.x, dimensions.y + dimensions.height / 2 - 3.0f},
             {dimensions.width + dimensions.x,
              dimensions.y + dimensions.height / 2 - 3.0f},
             6.0f, sliderColor);

  float sliderHeadX =
      ((value - minValue) / (maxValue - minValue)) * dimensions.width;

  DrawCircleV(
      {sliderHeadX + dimensions.x, dimensions.y + dimensions.height / 2 - 3.0f},
      10.0f, buttonColor);

  DrawCircleLinesEx(
      {sliderHeadX + dimensions.x, dimensions.y + dimensions.height / 2 - 3.0f},
      10.0f, 1.0f, sliderColor);

  if (CheckCollisionPointCircle(GetMousePosition(),
                                {sliderHeadX + dimensions.x,
                                 dimensions.y + dimensions.height / 2 - 3.0f},
                                10.0f) &&
      IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

    float t = (GetMouseX() - dimensions.x) / dimensions.width;

    // Clamp the mouse position to the slider bounds
    t = std::clamp(t, 0.0f, 1.0f);

    value = minValue + t * (maxValue - minValue);
  }
}
