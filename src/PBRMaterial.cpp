#pragma once
#include "Material.cpp"
#include "Color.cpp"
#include "TransformComponent.cpp"
#include "Texture.cpp"

struct PBRAttr {
  Color color;
  float32 metallic;
  float32 roughness;
};

struct PBRMaterial : public Material {
  PBRAttr* attr;
  uint32 mapIDs[4];

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

  virtual void render(UniformList& globalUniforms) override {
    glUseProgram(program); 

    for(int32 i = 0; i < 4; ++i) {
      glActiveTexture(GL_TEXTURE1 + i);
      glBindTexture(GL_TEXTURE_2D, mapIDs[i]);
    }

    uniform_tick_list(globalUniforms, program); 
    uniform_tick_list(uniforms, program);
  }

  PBRMaterial(TransformComponent* tc, AssetMap& map) {
    attr = new PBRAttr;

    set_color(0xFFFFFF);
    set_metallic(1.0f);
    set_roughness(0.2f);

    load_shaders("res/shaders/vertex_PBR.glsl", "res/shaders/fragment_PBR.glsl");

    mapIDs[0] = texture_create(map, "res/textures/T_Leather_D.jpg");
    mapIDs[1] = texture_create(map, "res/textures/T_Leather_R.jpg");
    mapIDs[2] = texture_create(map, "res/textures/T_Leather_AO.jpg");
    mapIDs[3] = texture_create(map, "res/textures/T_Leather_N.jpg");
    //mapIDs[4] = 0;//texture_create(map, "res/textures/T_Leather_Metallic.png");

    uniform(uniforms, "diffuseMap",   1);
    uniform(uniforms, "roughnessMap", 2);
    uniform(uniforms, "AOMap",        3);
    uniform(uniforms, "normalMap",    4);
    uniform(uniforms, "metallicMap",  5);
  
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

PBRMaterial* material_pbr(TransformComponent* tc, AssetMap& map) {
  return new PBRMaterial(tc, map);
}

