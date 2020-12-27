#pragma once
#include "OpenGL.cpp"
#include "Input.cpp"
#include "Math.cpp"


void app_start(OpenGLState* state, Input* input) {
  rect(state, { 0.0f,  0.0f,   0.0f}, 1, { 63.0f/255.0f, 136.0f/255.0f, 197.0f/255.0f});
  rect(state, { 1.0f,  4.0f,  -1.0f}, 1, {255.0f/255.0f, 210.0f/255.0f, 117.0f/255.0f});
  rect(state, {-4.0f, -2.0f,   3.0f}, 3, {104.0f/255.0f, 182.0f/255.0f, 132.0f/255.0f});
  rect(state, { 4.0f,  2.0f,   0.0f}, 2, {209.0f/255.0f, 64.0f/255.0f,  129.0f/255.0f});
}

bool app_tick(OpenGLState* state, Input* input, float64 dt) {
  Camera* c = state->camera;

  float32 x = c->position.x;
  float32 y = c->position.y;
  float32 z = c->position.z;

  float32 xr = c->rotation.x;
  float32 yr = c->rotation.y;
  float32 zr = c->rotation.z;

  const float32 s  = 10.0f * dt;
  const float32 sr = 10.0f * dt * d2r();

  float32 dMouseX = input->mouseXDiff * sr;
  float32 dMouseY = input->mouseYDiff * sr; 

  Vec3 camFront = vec3_normalize(vec3_forward(-c->rotation));
  Vec3 camRight = vec3_normalize(vec3_right  (-c->rotation));

  xr  = clamp(xr - dMouseY, pi() * -0.4999f, pi() * 0.4999f);
  yr -= dMouseX;

  Vec3 nextMove = {};
  nextMove.z = is_down(input, KEY_W) - is_down(input, KEY_S);
  nextMove.x = is_down(input, KEY_D) - is_down(input, KEY_A);
  nextMove = vec3_normalize(nextMove);

  Vec3 velocity = (nextMove.z * camFront * s) + (nextMove.x * camRight * s);
  x += velocity.x;
  y += velocity.y + (is_down(input, KEY_E) - is_down(input, KEY_Q)) * s;
  z += velocity.z;

  c->set_transform(state, {x,y,z}, {xr,yr,zr});
  return is_down(input, KEY_ESCAPE);
}


