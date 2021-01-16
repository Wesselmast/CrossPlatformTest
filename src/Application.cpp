#pragma once

#include "AppState.cpp"
#include "Camera.cpp"
#include "OpenGL.cpp"
#include "Input.cpp"
#include "Math.cpp"
#include "Terrain.cpp"

AppState* app_start(OpenGLState* state, Input* input) {
  AppState* app = (AppState*)malloc(sizeof(AppState));
  app->camera = create_camera(state);

  Transform terrainT = {{500.0f, -300.0f, 100.0f}, zero(), {100.0f, 250.0f, 100.0f}};
  terrain(state, terrainT, 2048);

  for(int x = 0; x < 10; ++x) {
    for(int y = 0; y < 10; ++y) {
      Vec3 p = {float32(x * 25) - 100.0f, float32(y * 25), 100.0f};
      PBR pbr;
      pbr.hexColor = 0xE26D5A;
      pbr.metallic = (float32)x / 10.0f;
      pbr.roughness= (float32)y / 10.0f;
      sphere(state, {p, zero(), one() * 10.0f}, pbr);
    }
  }

  Transform playerT = {{0.0f, 2.0f, 0.0f}, zero(), one() * 1.2f};
  app->player = cube(state, playerT, 0xD81E5B);

  PBR lightPBR;
  lightPBR.hexColor = 0xFFFFFF;
  lightPBR.metallic = 0.0f;
  lightPBR.roughness = 0.0f;
  app->light = sphere(state, {{700.0f, -100.0f, 200.0f}, zero(), one() * 6.0f}, lightPBR);

  register_key_down(input, KEY_F3, [](){ set_rendermode(RENDERMODE_WIREFRAME); });
  register_key_down(input, KEY_F4, [](){ set_rendermode(RENDERMODE_NORMAL);    });
  register_key_down(input, KEY_F5, [](){ set_rendermode(RENDERMODE_POINT);     });

  return app;
}

bool app_tick(OpenGLState* state, Input* input, AppState* app, float64 dt, float64 time) {
  Camera* c = app->camera;

  float32 x = c->position.x;
  float32 y = c->position.y;
  float32 z = c->position.z;

  float32 xr = c->rotation.x;
  float32 yr = c->rotation.y;
  float32 zr = c->rotation.z;

  const float32 sr = 10.0f  * dt;
  const float32 s  = 100.0f * dt;

  float32 dMouseX = input->mouseXDiff * sr;
  float32 dMouseY = input->mouseYDiff * sr; 

  Vec3 camFront = vec3_normalize(vec3_forward(-c->rotation));
  Vec3 camRight = vec3_normalize(vec3_right  (-c->rotation));

  xr  = clamp(xr - dMouseY, -89.999f, 89.999f);
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

  Entity* p = app->player;
  Vec3& pPos = p->transform.position;
  Vec3& pRot = p->transform.rotation;

  float32 speed = 1.0f;
  pRot.z += speed * dt *  5;
  pRot.x += speed * dt * 10;
  pRot.y += speed * dt * 15;
  p->set_transform(p->transform);

  float32 lSpeed = 0.9f, dist = 250.0f;
  Entity* l = app->light;
  Vec3 lPos = l->transform.position;

  lPos.y = sin(time * lSpeed) * dist;
  lPos.x = sin(time * lSpeed) * dist;
  lPos.z = cos(time * lSpeed) * dist;

  app->light->set_position(lPos);
  state->lightPos = lPos;

  return is_down(input, KEY_ESCAPE);
}

void app_end(AppState* app) {
  free(app->camera);
  free(app);
}
