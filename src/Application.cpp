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
#include "ScriptComponent.cpp"

void rotating_cube_tick(Input* input, Actor* actor, float64 dt, float64 time) {
  TransformComponent* rc = get_component<TransformComponent*>(actor, "transform");
  Vec3 rRC = rc->transform.rotation;
  rRC.x += 40.0f * dt;
  rRC.y += 30.0f * dt;
  rc->set_rotation(rRC);
}

void moving_light_tick(Input* input, Actor* actor, float64 dt, float64 time) {
  float32 lSpeed = 0.9f, dist = 250.0f;
  TransformComponent* l = get_component<TransformComponent*>(actor, "transform");
  Vec3 lPos = l->transform.position;

  lPos.y = sin(time * lSpeed) * dist;
  lPos.x = sin(time * lSpeed) * dist;
  lPos.z = cos(time * lSpeed) * dist;

  l->set_position(lPos);
}

void camera_movement(Input* input, Actor* actor, float64 dt, float64 time) {
  if(!globalConfinedCursor) return;

  TransformComponent* c = get_component<TransformComponent*>(actor, "transform");
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
}

Actor* actor_skybox(OpenGLState* state, AppState* app, CameraComponent* camera) {
  Actor* a = actor(app->actors);
  
  Batch batch;
  batch.mesh     = mesh_simple_inverted_cube(app->aLUT);
  batch.material = material_skybox(app->aLUT, state, camera); 
  batch.tc       = component_transform(zero_t()); 

  add_component(a, "transform", batch.tc);
  add_component(a, "renderer",  component_renderer(state->batches, batch));
  return a;
}

Actor* actor_generic(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor(app->actors); 
  add_component(a, "transform", component_transform(t));
  add_component(a, "renderer",  component_renderer(state->batches));
  return a;
}

Actor* actor_terrain(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a,  "renderer");
  
  r->set_mesh(mesh_terrain(app->aLUT, 512));
  r->set_material(material_terrain(app->aLUT));
  r->set_transform_component(tc);

  return a;
}

Actor* actor_pbr_sphere(OpenGLState* state, AppState* app, const Transform& t, float32 roughness, float32 metallic) {
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");

  PBRMaterial* pbr = material_pbr(app->aLUT); 
  pbr->set_roughness(roughness);
  pbr->set_metallic(metallic);

  r->set_mesh(mesh_sphere(app->aLUT));
  r->set_material(pbr);
  r->set_transform_component(tc);

  return a;
}

Actor* actor_pbr_cube(OpenGLState* state, AppState* app, const Transform& t, float32 roughness, float32 metallic) {
  Actor* a = actor_pbr_sphere(state, app, t, roughness, metallic); 

  RendererComponent* r = get_component<RendererComponent*>(a, "renderer");
  r->set_mesh(mesh_cube(app->aLUT));

  add_component(a, "rotator", component_script(app->scripts, a, &rotating_cube_tick));
  return a;
}

Actor* actor_camera(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor(app->actors);

  TransformComponent* tc = component_transform(t);
  add_component(a, "transform", tc);
  add_component(a, "camera", component_camera(state, tc));
  add_component(a, "cameraMovement", component_script(app->scripts, a, &camera_movement));

  return a;
}

Actor* actor_water_plane(OpenGLState* state, AppState* app, const Transform& t) {
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");

  r->set_mesh(mesh_plane(app->aLUT));
  r->set_material(material_unlit(app->aLUT, 0x0000FF, "material_unlit_water"));
  r->set_transform_component(tc);

  return a;
}

Actor* actor_point_light(OpenGLState* state, AppState* app, const Vec3& v, const LightAttributes& attr) {
  Transform t = {v, zero(), one()};
  Actor* a = actor_generic(state, app, t);

  TransformComponent* tc = get_component<TransformComponent*>(a, "transform"); 
  RendererComponent*  r  = get_component<RendererComponent*>(a, "renderer");

  r->set_mesh(mesh_cube(app->aLUT));
  r->set_material(material_unlit(app->aLUT, attr.hexcolor));
  r->set_transform_component(tc);

  add_component(a, "light", component_light(state, attr, tc));
  return a;
}

AppState* app_start(OpenGLState* state, Input* input) {
  AppState* app = new AppState;
  app->camera = actor_camera(state, app, zero_t());

  actor_skybox(state, app, get_component<CameraComponent*>(app->camera, "camera"));
  
  
  Transform terrainT = {{ 500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}};
  actor_terrain(state, app, terrainT); 
  Transform waterT = terrainT;
  waterT.scale = waterT.scale * 10.0f;
  waterT.position.y = -200.0f;
  actor_water_plane(state, app, waterT);

  //actor_terrain(state, app, {{ 500.0f, -300.0f, 1100.0f}, zero(), {100.0f, 250.0f, 100.0f}});
  //actor_terrain(state, app, {{1500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}});
  //actor_terrain(state, app, {{-500.0f, -300.0f, 100.0f},  zero(), {100.0f, 250.0f, 100.0f}});
  
  actor_pbr_cube(state, app, {{0.0f, -50.0f, 50.0f}, zero(), one() * 5.0f}, 0.9f, 1.0f);

  const int32 res = 5;
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

  Actor* light = actor_point_light(state, app, zero(), {0xFFFFDD, 35000.0f});
  add_component(light, "movingLight", component_script(app->scripts, light, &moving_light_tick));
  actor_point_light(state, app, zero(), {0xFFDD88, 55000.0f});
  actor_point_light(state, app, {200.0f, 0.0f, 0.0f}, {0x884433, 45000.0f}); 

  register_key_down(input, KEY_ALT, []() { confine_cursor(false); });
  register_key_up(  input, KEY_ALT, []() { confine_cursor(true);  });

  register_key_down(input, KEY_F3,  [](){ set_rendermode(RENDERMODE_WIREFRAME); });
  register_key_down(input, KEY_F4,  [](){ set_rendermode(RENDERMODE_NORMAL);    });
  register_key_down(input, KEY_F5,  [](){ set_rendermode(RENDERMODE_POINT);     });

  return app;
}

bool app_tick(OpenGLState* state, Input* input, AppState* app, float64 dt, float64 time) {
  for(ScriptComponent* sc : app->scripts) {
    sc->tick(input, dt, time);
  } 

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
