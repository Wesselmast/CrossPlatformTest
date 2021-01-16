#pragma once
#include "OpenGL.cpp"
#include "Camera.cpp"
#include "Light.cpp"

struct AppState {
  Entity* player;
  Light* light;
  Camera* camera;
};
