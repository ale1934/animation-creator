#include "animation.hpp"
#include "frame.hpp"
#include "gif.h"
#include "settings.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <raylib.h>
#include <string>
#include <sys/types.h>

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

void Animation::update() {
  // Toggle animation playback
  if (IsKeyPressed(KEY_SPACE)) {
    animating = !animating;
    frameTimer = 0.0f;
  }

  // Handle mouse input on the current frame
  if (!animating) {
    frames.at(currentFrame)->handleMouse(canDraw);

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_E)) {
      if (currentFrame + 1 < frames.size()) {
        currentFrame++;
      }
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_Q)) {
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

    // Duplicate frame after current
    if (IsKeyPressed(KEY_P)) {
      frames.insert(frames.begin() + currentFrame + 1,
                    new Frame(animationResolution));

      frames.at(currentFrame + 1)
          ->setImage(frames.at(currentFrame)->getImage());
      currentFrame++;
    }

    // Change Color using keys 1-7
    int key = GetKeyPressed();

    if (std::find(std::begin(keys), std::end(keys), key) != std::end(keys)) {
      g_settings.drawColor = key - 49;
    }

    // Save Animation
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
      exportGif("animation.gif");
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
  frames.at(currentFrame)->renderFrame(currentFrame);

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

  DrawCircleV({20, 20}, 15.0f, g_settings.palette[g_settings.drawColor]);
  DrawCircleLinesEx({20, 20}, 15.0f, 2.0f, BLACK);
}

void Animation::setFrame(int frame, Texture2D image) {
  if (frame > frames.size() - 1)
    frames.push_back(new Frame(animationResolution));

  frames[frame]->setImage(LoadImageFromTexture(image));
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
