#pragma once

#include "OpenGL.cpp"
#include "AppState.cpp"
#include "Input.cpp"
#include "Math.cpp"
#include "LightComponent.cpp"
#include "CameraComponent.cpp"

#include "Actor.cpp"
#include "PBRMaterial.cpp"
#include "SkyboxMaterial.cpp"
#include "TerrainMaterial.cpp"
#include "RendererComponent.cpp"
#include "TransformComponent.cpp"

Actor* actor_skybox(OpenGLState* state, AppState* app, CameraComponent* camera) {
  Actor* a = actor(app->actors);
  SkyboxMaterial* skyMat = material_skybox(state, camera);
  add_component(a, "renderer", component_renderer(state->renderers, mesh_simple_inverted_cube(app->aLUT), skyMat));
  return a;
}

Actor* actor_generic(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor(app->actors); 
  add_component(a, "transform", component_transform(t));
  add_component(a, "renderer",  component_renderer(state->renderers));
  return a;
}

Actor* actor_terrain(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");
  
  r->set_mesh(mesh_terrain(app->aLUT, 1024));
  r->set_material(material_terrain(tc));

  return a;
}

Actor* actor_pbr_sphere(OpenGLState* state, AppState* app, const Transform& t, float32 roughness, float32 metallic) {
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");

  r->set_mesh(mesh_sphere(app->aLUT));
  PBRMaterial* pbr = material_pbr(tc); 
  pbr->set_roughness(roughness);
  pbr->set_metallic(metallic);
  r->set_material(pbr);

  return a;
}

Actor* actor_pbr_cube(OpenGLState* state, AppState* app, const Transform& t, float32 roughness, float32 metallic) {
  Actor* a = actor_pbr_sphere(state, app, t, roughness, metallic); 

  RendererComponent* r = get_component<RendererComponent*>(a, "renderer");
  r->set_mesh(mesh_cube(app->aLUT));

  return a;
}

Actor* actor_camera(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor(app->actors);

  TransformComponent* tc = component_transform(t);
  add_component(a, "transform", tc);
  add_component(a, "camera", component_camera(state, tc));

  return a;
}

Actor* actor_point_light(OpenGLState* state, AppState* app, const Vec3& v, const LightAttributes& attr) {
  Transform t = {v, zero(), one()};
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");

  r->set_mesh(mesh_cube(app->aLUT));
  r->set_material(material_unlit(tc, attr.hexcolor));

  add_component(a, "light", component_light(state, attr, tc));
  return a;
}

AppState* app_start(OpenGLState* state, Input* input) {
  AppState* app = new AppState;
  app->camera = actor_camera(state, app, zero_t());

  actor_skybox(state, app, get_component<CameraComponent*>(app->camera, "camera"));
  actor_terrain(state, app, {{ 500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}});
  actor_terrain(state, app, {{ 500.0f, -300.0f, 1100.0f}, zero(), {100.0f, 250.0f, 100.0f}});
  actor_terrain(state, app, {{1500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}});
  actor_terrain(state, app, {{-500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}});
  
  app->rotatingCube = actor_pbr_cube(state, app, {{0.0f, -50.0f, 50.0f}, zero(), one() * 5.0f}, 0.9f, 1.0f);

  const int32 res = 10;
  for(int x = 0; x < res; ++x) {
    for(int y = 0; y < res; ++y) {
      Transform tS = zero_t();
      tS.position = {float32(x * 25) - 100.0f, float32(y * 25), 100.0f};
      tS.scale = tS.scale * 10.0f;
      float32 metallic  = (float32)x / (float32)res;
      float32 roughness = (float32)y / (float32)res;

      actor_pbr_sphere(state, app, tS, roughness, metallic);
    }
  }

  app->movingLight = actor_point_light(state, app, zero(), {0xFFFFDD, 35000.0f});
  actor_point_light(state, app, zero(), {0xFFDD88, 55000.0f});
  actor_point_light(state, app, {200.0f, 0.0f, 0.0f}, {0x884433, 45000.0f}); 

  register_key_down(input, KEY_F3, [](){ set_rendermode(RENDERMODE_WIREFRAME); });
  register_key_down(input, KEY_F4, [](){ set_rendermode(RENDERMODE_NORMAL);    });
  register_key_down(input, KEY_F5, [](){ set_rendermode(RENDERMODE_POINT);     });

  return app;
}

bool app_tick(OpenGLState* state, Input* input, AppState* app, float64 dt, float64 time) {
  TransformComponent* c = get_component<TransformComponent*>(app->camera, "transform");
  Transform& ct = c->transform;

  float32 x = ct.position.x;
  float32 y = ct.position.y;
  float32 z = ct.position.z;

  float32 xr = ct.rotation.x;
  float32 yr = ct.rotation.y;
  float32 zr = ct.rotation.z;

  const float32 sr = 10.0f  * dt;
  const float32 s  = 100.0f * dt;

  float32 dMouseX = input->mouseXDiff * sr;
  float32 dMouseY = input->mouseYDiff * sr; 

  Vec3 camFront = vec3_normalize(vec3_forward(-ct.rotation));
  Vec3 camRight = vec3_normalize(vec3_right  (-ct.rotation));

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

  c->set_transform({{x,y,z}, {xr,yr,zr}, one()});

  float32 lSpeed = 0.9f, dist = 250.0f;
  TransformComponent* l = get_component<TransformComponent*>(app->movingLight, "transform");
  Vec3 lPos = l->transform.position;

  lPos.y = sin(time * lSpeed) * dist;
  lPos.x = sin(time * lSpeed) * dist;
  lPos.z = cos(time * lSpeed) * dist;

  l->set_position(lPos);

  TransformComponent* rc = get_component<TransformComponent*>(app->rotatingCube, "transform");
  Vec3 rRC = rc->transform.rotation;
  rRC.x += 40.0f * dt;
  rc->set_rotation(rRC);

  return is_down(input, KEY_ESCAPE);
}

void app_end(AppState* app) {
  while(!app->actors.empty()) {
    //TODO: this causes a segfault.. look into why
    //delete app->actors.front();
    app->actors.pop_front();
  }
  for(auto itr : app->aLUT) {
    delete itr.second;
  }
  app->aLUT.clear();

  delete app;
}
