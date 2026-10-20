#pragma once

#include <raylib.h>
#include <string>
#include <vector>

#define FONTSIZE 16
#define SEGMENTS 16

struct ColorWheelState {
  bool open = false;
  int selectedIndex = -1;
  Vector2 center = {0, 0};
  float outerRadius = 80.0f;
  float innerRadius = 30.0f;
};

class Widgets {
public:
  // returns true when mouse is over panel
  bool static Panel(Rectangle dimensions, Color color, float roundness,
                    bool borderEnabled, Color borderColor,
                    float borderThickness);

  void static Label(Font UiFont, Rectangle dimensions, std::string label,
                    Color color, float roundness, bool borderEnabled,
                    Color borderColor, float borderThickness);

  bool static Button(Font UiFont, Rectangle dimensions, std::string label,
                     Color color, float roundness, bool borderEnabled,
                     Color borderColor, float borderThickness);

  void static ColorEdit(Rectangle dimensions, Color &color,
                        std::vector<Color> &palette, ColorWheelState &wheel);

  bool static ColorWheel(std::vector<Color> &palette, ColorWheelState &wheel);

  void static FloatSlider(Rectangle dimensions, Color sliderColor,
                          Color buttonColor, float &value, float minValue,
                          float maxValue);
};
