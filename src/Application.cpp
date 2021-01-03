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

  rect(state, {zero(),                zero(), one()},     0xFE5F55); 
  rect(state, {{ 1.0f,  4.0f, -1.0f}, zero(), one()},     0x6665DD); 
  rect(state, {{-4.0f, -2.0f,  3.0f}, zero(), one() * 3}, 0x0B7A75); 

  Transform terrainT = {{0.0f, -30.0f, 0.0f}, zero(), one() * 7};
  terrain(state, terrainT, 512, 2.5f);

  Transform playerT = {{4.0f, 2.0f, 0.0f}, zero(), one() * 1.2f};
  app->player = cube(state, playerT, 0xD81E5B);

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

  const float32 s  = 10.0f * dt;

  float32 dMouseX = input->mouseXDiff * s;
  float32 dMouseY = input->mouseYDiff * s; 

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

  float32 speed = 8.0f, dist = 2.0f, rot = 12.0f;
  //pPos.x = sin(time * speed) * dist;
  //pRot.x = sin(time * speed) * rot;
  pRot.y += speed * dt; //cos(time * speed) * rot;
  p->set_transform(p->transform);

  return is_down(input, KEY_ESCAPE);
}

void app_end(AppState* app) {
  free(app->camera);
  free(app);
}
