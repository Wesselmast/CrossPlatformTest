#pragma once
#include "OpenGL.cpp"
#include "Camera.cpp"

struct AppState {
  Entity* player;
  Entity* light;
  Camera* camera;
};
