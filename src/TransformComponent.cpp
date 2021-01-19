#pragma once

#include "Components.cpp"
#include "Math.cpp"

struct TransformComponent : public Component {
  Mat4 modelMatrix;
  Mat4 normalMatrix;
  Transform transform;

  void set_scale(const Vec3& scale) {
    transform.scale = scale;
    set_transform(transform);
  }

  void set_rotation(const Vec3& rotation) {
    transform.rotation = rotation;
    set_transform(transform);
  }

  void set_position(const Vec3& position) {
    transform.position = position;
    set_transform(transform);
  }

  void set_transform(const Transform& t) {
    modelMatrix = mat4_scaling(t.scale) * mat4_euler_rotation(t.rotation) * mat4_translation(t.position);
    normalMatrix = mat4_transpose(mat4_inverse(modelMatrix)); 
    this->transform = t;
  }

  TransformComponent(const Transform& t) {
    set_transform(t);
  }
};

TransformComponent* component_transform(const Transform& t) {
  return new TransformComponent(t);
} 

