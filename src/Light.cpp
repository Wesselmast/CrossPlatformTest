#pragma once

#include "Color.cpp"
#include "Math.cpp"
#include "Types.cpp" 
#include "Entity.cpp"
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

  Entity* gizmo;

  void set_position(const Vec3& p) {
    gizmo->set_position(p);
    position = p;
  }

  Light* init(OpenGLState* state, const LightAttributes& attr, int32 index) {
    PBR pbr;
    pbr.hexColor = attr.hexcolor;
    pbr.metallic = 0.0f;
    pbr.roughness = 0.0f;

    position = attr.position;
    radius   = attr.radius;
    color    = hex_to_color(attr.hexcolor);

    gizmo = sphere(state, {position, zero(), one()}, pbr);

    std::string baseStr = "lights[" + std::to_string(index) + "]";
    
    std::string posStr = baseStr + ".position";
    std::string colStr = baseStr + ".color";
    std::string radStr = baseStr + ".radius";

    uniform(state->globalUniforms,  posStr, &position);
    uniform(state->globalUniforms, colStr, &color);
    uniform(state->globalUniforms, radStr, &radius);
    
    state->amtOfLights = index + 1;
    return this;
  }
};

Light* point_light(OpenGLState* state, const LightAttributes& attr) {
  static int32 index = 0;
  return ((Light*)malloc(sizeof(Light)))->init(state, attr, index++);
}

