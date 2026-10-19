#pragma once

#include "frame.hpp"
#include <raylib.h>
#include <string>
#include <vector>

class Animation {
private:
  std::vector<Frame *> frames;
  Vector2 animationResolution;
  int framesPerSecond = 12;
  int currentFrame = 0;

  float frameTimer = 0.0f;
  bool animating = false;
  bool canDraw = true;

  Shader onionShader;
  int tintLoc;

public:
  Animation(Vector2 res, int fps, int frameCount);

  void update();
  void render();

  int getCurrentFrame() const;

  void clearMousePos();

  void exportPNGs(const std::string &dir);
  void exportGif(const std::string &path);
};
