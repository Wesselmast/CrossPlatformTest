#pragma once
#include "Types.cpp"
#include "Math.cpp"

struct Component {
  virtual void tick(OpenGLState* state) {}
  virtual ~Component() {}
};

