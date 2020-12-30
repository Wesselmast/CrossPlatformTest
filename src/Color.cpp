#pragma once

#include "Types.cpp"

struct Color {
  float32 r;
  float32 g;
  float32 b;
};

Color hex_to_color(int32 hex) {
  Color color;
  color.r = (hex >> 16 & 0xFF) / 255.0f;
  color.g = (hex >>  8 & 0xFF) / 255.0f;
  color.b = (hex >>  0 & 0xFF) / 255.0f;
  return color;
}
