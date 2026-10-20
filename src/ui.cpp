#include "ui.hpp"
#include "settings.hpp"
#include "widgets.hpp"
#include <format>
#include <raylib.h>
#include <string>

// Constructor
UI::UI(Vector2 dim, Animation &anim) {
  dimensions = dim;
  currentAnimation = &anim;
  toolBarRect = {25, 10, dimensions.x - 50, dimensions.y / 16};

  // Load Font
  UiFont =
      LoadFontEx("resources/CascadiaMono/CaskaydiaMonoNerdFont-Regular.ttf",
                 FONTSIZE, 0, 250);

  if (UiFont.texture.id == 0)
    TraceLog(LOG_ERROR, "font failed to load");
}

// Update (get key presses)
void UI::update() {
  if (IsKeyPressed(KEY_T)) {
    showTools = !showTools;
  }
}

// Render all ui elements if they should be shown
void UI::render() {
  if (showTools)
    toolBar();

  // Center label for displaying current frame
  DrawTextEx(
      UiFont,
      ("Frame: " + std::to_string(currentAnimation->getCurrentFrame() + 1))
          .c_str(),
      {toolBarRect.x + toolBarRect.width / 2 - 40, dimensions.y - 20}, FONTSIZE,
      0.0f, g_settings.UITextColor);
}

void UI::toolBar() {
  // Main Panel
  if (Widgets::Panel(toolBarRect, g_settings.UIBackground, 0.3f, true,
                     g_settings.UIBackgroundBorder, 1.0f)) {
    needsInput = true;
  } else {
    needsInput = false;
  }

  // Color Pallete on tool bar top
  Widgets::ColorEdit({toolBarRect.x + 10, 15, 90, 30}, g_settings.drawColor,
                     currentPallete, wheel);

  if (Widgets::ColorWheel(currentPallete, wheel)) {
    needsInput = true;
  }

  // Slider select for line thickness
  Widgets::FloatSlider({toolBarRect.x + 120, 18, 80, 30}, GRAY,
                       g_settings.UIBackground, g_settings.lineThickness, 2.0f,
                       40.0f);

  // Display current line thickness
  std::string lineThicknessText = std::format("{:g}", g_settings.lineThickness);
  Widgets::Label(UiFont,
                 {toolBarRect.x + toolBarRect.width / 2 - 240, 15, 120, 30},
                 "size: " + lineThicknessText, g_settings.UIBackground, 0.3f,
                 false, g_settings.UIBackgroundBorder, 1.0f);

  if (Widgets::Button(UiFont,
                      {toolBarRect.x + toolBarRect.width - 90, 15, 80, 30},
                      "Save", g_settings.UIBackground, 0.3f, true,
                      g_settings.UIBackgroundBorder, 1.0f)) {
    currentAnimation->exportGif("animation.gif");
    currentAnimation->exportPNGs("export");
  }
}

bool UI::getNeedInput() const { return needsInput; }
