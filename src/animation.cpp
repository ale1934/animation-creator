#include "animation.hpp"
#include "gif.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <raylib.h>
#include <string>

static const char *onionFS = R"(
#version 330
in vec2 fragTexCoord;
uniform sampler2D texture0;
uniform vec4 tint;
out vec4 finalColor;

void main() {
    vec4 texel = texture(texture0, fragTexCoord);
    float ink = 1.0 - dot(texel.rgb, vec3(0.299, 0.587, 0.114));
    finalColor = vec4(tint.rgb, ink * tint.a);
}
)";

Animation::Animation(Vector2 res, int fps, int frameCount) {
  animationResolution = res;
  framesPerSecond = fps;
  currentFrame = 0;
  for (int i = 0; i < frameCount; i++) {
    frames.push_back(new Frame(res));
  }

  onionShader = LoadShaderFromMemory(nullptr, onionFS);
  tintLoc = GetShaderLocation(onionShader, "tint");
}

void Animation::update(bool canDraw) {
  // Toggle animation playback
  if (IsKeyPressed(KEY_SPACE)) {
    animating = !animating;
    frameTimer = 0.0f;
  }

  // Handle mouse input on the current frame
  if (!animating) {
    frames.at(currentFrame)->handleMouse(canDraw);

    if (IsKeyPressed(KEY_RIGHT)) {
      if (currentFrame + 1 < frames.size()) {
        currentFrame++;
      }
    } else if (IsKeyPressed(KEY_LEFT)) {
      if (currentFrame > 0) {
        currentFrame--;
      }
    }

    // Add frame after current
    if (IsKeyPressed(KEY_A)) {
      frames.insert(frames.begin() + currentFrame + 1,
                    new Frame(animationResolution));

      currentFrame++;
    }

    // Delete current frame
    if (IsKeyPressed(KEY_D)) {
      if (!currentFrame - 1 < 0) {
        frames.erase(frames.begin() + currentFrame);
        currentFrame--;
      }
    }
  }

  // Advance animation based on elapsed time
  if (animating) {
    frameTimer += GetFrameTime();

    float frameDuration = 1.0f / framesPerSecond;

    if (frameTimer >= frameDuration) {
      currentFrame = (currentFrame + 1) % frames.size();
      frameTimer -= frameDuration;
    }
  }
}

void Animation::render() {
  frames.at(currentFrame)->renderFrame();

  if (!animating) {
    // Second layer (two frames back), drawn first so it sits underneath
    if (currentFrame - 2 >= 0) {
      frames[currentFrame - 2]->renderOnion(onionShader, tintLoc,
                                            Color{0, 0, 255, 30});
    }
    // First layer (previous frame), drawn on top
    if (currentFrame - 1 >= 0) {
      frames[currentFrame - 1]->renderOnion(onionShader, tintLoc,
                                            Color{255, 0, 0, 30});
    }
  }
}

void Animation::exportPNGs(const std::string &dir) {
  std::filesystem::create_directories(dir);

  for (size_t i = 0; i < frames.size(); i++) {
    Image img = frames[i]->getImage();

    char name[64];
    snprintf(name, sizeof(name), "/frame_%03zu.png", i + 1);
    ExportImage(img, (dir + name).c_str());

    UnloadImage(img);
  }
}

void Animation::exportGif(const std::string &path) {
  if (frames.empty())
    return;

  // GIF delays are in hundredths of a second
  uint32_t delay = std::max(1, 100 / framesPerSecond);

  GifWriter writer;
  bool started = false;

  for (Frame *frame : frames) {
    Image img = frame->getImage();

    if (!started) {
      GifBegin(&writer, path.c_str(), img.width, img.height, delay);
      started = true;
    }

    GifWriteFrame(&writer, (const uint8_t *)img.data, img.width, img.height,
                  delay);
    UnloadImage(img);
  }

  GifEnd(&writer);
}

int Animation::getCurrentFrame() const { return currentFrame; }
