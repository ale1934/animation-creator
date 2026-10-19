#include "ui.hpp"
#include "settings.hpp"
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
}

void UI::toolBar() {
  // Main Panel
  if (Widgets::Panel(toolBarRect, toolBarColor, 0.3f, true, toolBarBorderColor,
                     1.0f)) {
    needsInput = true;
  } else {
    needsInput = false;
  }

  // Color Pallete on tool bar left
  Widgets::ColorEdit({toolBarRect.x + 30, 15, 200, 30}, g_settings.drawColor,
                     currentPallete, wheel);

  if (Widgets::ColorWheel(currentPallete, wheel)) {
    needsInput = true;
  }

  // Center label for displaying current frame
  Widgets::Label(
      UiFont, {toolBarRect.x + toolBarRect.width / 2 - 40, 15, 80, 30},
      "Frame: " + std::to_string(currentAnimation->getCurrentFrame() + 1),
      toolBarColor, 0.3f, true, toolBarBorderColor, 1.0f);

  if (Widgets::Button(
          UiFont, {toolBarRect.x + toolBarRect.width - 90, 15, 80, 30}, "Save",
          toolBarColor, 0.3f, true, toolBarBorderColor, 1.0f)) {
    currentAnimation->exportGif("animation.gif");
    currentAnimation->exportPNGs("export");
  }
}

bool UI::getNeedInput() const { return needsInput; }
