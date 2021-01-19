#pragma once

#include "AppState.cpp"
#include "Camera.cpp"
#include "OpenGL.cpp"
#include "Input.cpp"
#include "Math.cpp"
#include "Light.cpp"

#include "Actor.cpp"
#include "PBRMaterial.cpp"
#include "SkyboxMaterial.cpp"
#include "TerrainMaterial.cpp"
#include "RendererComponent.cpp"
#include "TransformComponent.cpp"

Actor* actor_skybox(OpenGLState* state, Camera* camera) {
  Actor* a = actor(state);
  SkyboxMaterial* skyMat = material_skybox(state, camera);
  add_component(a, "renderer", component_renderer(mesh_simple_inverted_cube(), skyMat));
  return a;
}

Actor* actor_generic(OpenGLState* state, const Transform& t) {
  Actor* a = actor(state); 

  TransformComponent* tc = component_transform(t);
  add_component(a, "transform", tc);

  //TODO: this is not pretty!
  add_component(a, "renderer",  component_renderer(mesh_cube(), material_pbr(tc)));

  return a;
}

Actor* actor_terrain(OpenGLState* state, const Transform& t) {
  Actor* a = actor_generic(state, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");
  
  r->set_mesh(mesh_terrain(256));
  r->set_material(material_terrain(tc));

  return a;
}

Actor* actor_pbr_sphere(OpenGLState* state, const Transform& t, Mesh* sphere, float32 roughness, float32 metallic) {
  Actor* a = actor_generic(state, t);

  RendererComponent* r = get_component<RendererComponent*>(a, "renderer");

  r->set_mesh(sphere);
  PBRMaterial* pbr = (PBRMaterial*)r->material;
  pbr->set_roughness(roughness);
  pbr->set_metallic(metallic);

  return a;
}


AppState* app_start(OpenGLState* state, Input* input) {
  AppState* app = (AppState*)malloc(sizeof(AppState));
  app->camera = create_camera(state);
  app->sphere = mesh_sphere();

  actor_skybox(state, app->camera);
  actor_terrain(state, {{500.0f, -300.0f, 100.0f}, zero(), {100.0f, 250.0f, 100.0f}});

  const int32 res = 10;
  for(int x = 0; x < res; ++x) {
    for(int y = 0; y < res; ++y) {
      Transform tS = zero_t();
      tS.position = {float32(x * 25) - 100.0f, float32(y * 25), 100.0f};
      tS.scale = tS.scale * 10.0f;
      float32 metallic  = (float32)x / (float32)res;
      float32 roughness = (float32)y / (float32)res;

      actor_pbr_sphere(state, tS, app->sphere, roughness, metallic);
    }
  }

  app->light = point_light(state, {0xFFFFFF, 35000.0f, zero()});
  point_light(state, {0xFFFFFF, 55000.0f, zero()});
  point_light(state, {0xFFFFFF, 45000.0f, {200.0f, 0.0f, 0.0f}});

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

  float32 lSpeed = 0.9f, dist = 250.0f;
  Light* l = app->light;
  Vec3 lPos = l->position;

  lPos.y = sin(time * lSpeed) * dist;
  lPos.x = sin(time * lSpeed) * dist;
  lPos.z = cos(time * lSpeed) * dist;

  app->light->set_position(lPos);
  return is_down(input, KEY_ESCAPE);
}

void app_end(AppState* app) {
  free(app->camera);
  free(app->light);
  free(app->sphere);
  free(app);
}
