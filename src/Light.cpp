#pragma once

#include "Color.cpp"
#include "Math.cpp"
#include "Types.cpp" 

#include "PBRMaterial.cpp"
#include "TransformComponent.cpp"
#include "RendererComponent.cpp"

#include "AppState.cpp"

#include <string>

struct LightAttributes {
  int32 hexcolor;
  float32 radius;
  Vec3 position;
};

struct Light {
  Color color;
  float32 radius;
  Vec3 position;

  TransformComponent* gizmoT;

  void set_position(const Vec3& p) {
    gizmoT->set_position(p);
    position = p;
  }

  Light(OpenGLState* state, AppState* app, const LightAttributes& attr, int32 index) {
    position = attr.position;
    radius   = attr.radius;
    color    = hex_to_color(attr.hexcolor);

    Actor* gizmo = actor(app->actors);
    gizmoT = component_transform({position, zero(), one()});
    add_component(gizmo, "transform", gizmoT);
    
    PBRMaterial* pbr = material_pbr(gizmoT);
    pbr->set_color(attr.hexcolor);
    pbr->set_metallic(0.0f);
    pbr->set_roughness(0.0f);

    add_component(gizmo, "renderer", component_renderer(state->renderers, mesh_sphere(app->aLUT), pbr));

    std::string baseStr = "lights[" + std::to_string(index) + "]";
    
    std::string posStr = baseStr + ".position";
    std::string colStr = baseStr + ".color";
    std::string radStr = baseStr + ".radius";

    uniform(state->globalUniforms, posStr, &position);
    uniform(state->globalUniforms, colStr, &color);
    uniform(state->globalUniforms, radStr, &radius);
    
    state->amtOfLights = index + 1;
  }
};

//TODO: rework lights a bit so this doesn't leak

Light* point_light(OpenGLState* state, AppState* app, const LightAttributes& attr) {
  static int32 index = 0;
  return new Light(state, app, attr, index++);
}

