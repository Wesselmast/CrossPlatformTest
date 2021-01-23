#pragma once
#include "Math.cpp"

struct Camera {
  Mat4 viewMatrix;
  Mat4 projMatrix;
  Vec3 position;
  Vec3 rotation;

  const Mat4 get_view_projection() const {
    return viewMatrix * projMatrix;
  }

  Mat4 calculate_projection(uint16 w, uint16 h) {
    return mat4_perspective_fov(68.0f, (float32)w/(float32)h, 0.1f, 2500.0f);
  }

  void set_transform(OpenGLState* state, const Vec3& position, const Vec3& rotation) {
    viewMatrix = mat4_translation(-position) * mat4_euler_rotation(rotation);
    projMatrix = calculate_projection(state->windowWidth, state->windowHeight);
    this->position = position;
    this->rotation = rotation;
    state->vp = get_view_projection();
  }

  Camera(OpenGLState* state) {
    set_transform(state, zero(), zero());
  }
};

Camera* create_camera(OpenGLState* state) {
  return new Camera(state);
}
