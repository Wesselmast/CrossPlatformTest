#pragma once

#include "Components.cpp"
#include "Math.cpp"

#include "TransformComponent.cpp"

struct CameraComponent : public Component {
  Mat4 viewMatrix;
  Mat4 projMatrix;
  Vec3* position;
  Vec3* rotation;

  const Mat4 get_view_projection() const {
    return viewMatrix * projMatrix;
  }

  Mat4 calculate_projection(uint16 w, uint16 h) {
    return mat4_perspective_fov(68.0f, (float32)w/(float32)h, 0.1f, 2500.0f);
  }

  void update(OpenGLState* state) {
    viewMatrix = mat4_translation(-(*position)) * mat4_euler_rotation(*rotation);
    projMatrix = calculate_projection(state->windowWidth, state->windowHeight);
    state->vp = get_view_projection();
    state->cameraPos = *position;
  }

  CameraComponent(OpenGLState* state, TransformComponent* tc) {
    position = &tc->transform.position;
    rotation = &tc->transform.rotation;
    update(state);
  }
};

CameraComponent* component_camera(OpenGLState* state, TransformComponent* tc) {
  return new CameraComponent(state, tc);
}
