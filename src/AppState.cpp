#pragma once
#include "OpenGL.cpp"
#include "Camera.cpp"
#include "Light.cpp"

struct AppState {
  Light* light;
  Mesh* sphere;
  Camera* camera;
};
