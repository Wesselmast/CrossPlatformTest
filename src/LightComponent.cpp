#pragma once

#include "Components.cpp"
#include "Math.cpp"
#include "Types.cpp" 

#include "TransformComponent.cpp"

#include <string>

struct LightAttributes {
  int32 hexcolor;
  float32 radius;
};

struct LightComponent : public Component {
  Color color;
  float32 radius;

  LightComponent(OpenGLState* state, const LightAttributes& attr, TransformComponent* tc) {
    radius   = attr.radius;
    color    = hex_to_color(attr.hexcolor);

    std::string baseStr = "lights[" + std::to_string(state->amtOfLights) + "]";
    
    std::string posStr = baseStr + ".position";
    std::string colStr = baseStr + ".color";
    std::string radStr = baseStr + ".radius";

    uniform(state->globalUniforms, posStr, &tc->transform.position);
    uniform(state->globalUniforms, colStr, &color);
    uniform(state->globalUniforms, radStr, &radius);
    
    state->amtOfLights++;
  }
};

LightComponent* component_light(OpenGLState* state, const LightAttributes& attr, TransformComponent* tc) {
  return new LightComponent(state, attr, tc);
}

