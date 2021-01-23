#pragma once
#include "Material.cpp"
#include "Color.cpp"
#include "TransformComponent.cpp"

struct PBRAttr {
  Color color;
  float32 metallic;
  float32 roughness;
};

struct PBRMaterial : public Material {
  PBRAttr* attr;

  void set_color(int32 hex) {
    attr->color = hex_to_color(hex);
  }

  void set_roughness(float32 roughness) {
    attr->roughness = roughness;
  }

  void set_metallic(float32 metallic) {
    attr->metallic = metallic;
  }

  void swap_texture() {} //example (for when textures get here)

  PBRMaterial(TransformComponent* tc) {
    attr = new PBRAttr;

    set_color(0xFFFFFF);
    set_metallic(1.0f);
    set_roughness(0.2f);

    load_shaders("res/shaders/vertex_PBR.glsl", "res/shaders/fragment_PBR.glsl");
  
    uniform(uniforms, "albedo",    &attr->color);
    uniform(uniforms, "metallic",  &attr->metallic);
    uniform(uniforms, "roughness", &attr->roughness);
    uniform(uniforms, "model",     &tc->modelMatrix);
    uniform(uniforms, "normalMat", &tc->normalMatrix);
  }

  ~PBRMaterial() {
    delete attr;
  }
};

PBRMaterial* material_pbr(TransformComponent* tc) {
  return new PBRMaterial(tc);
}

