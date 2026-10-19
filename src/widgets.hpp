#pragma once

#include <raylib.h>
#include <string>

#define FONTSIZE 16
#define SEGMENTS 16

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
};
