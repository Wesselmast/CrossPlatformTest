#pragma once

#include "Material.cpp"
#include "CameraComponent.cpp"
#include "Texture.cpp"

struct SkyboxMaterial : public Material {
  uint32 textureID;

  SkyboxMaterial(AssetMap& map, OpenGLState* state, CameraComponent* camera) {
    load_shaders("res/shaders/skybox_V.glsl", "res/shaders/skybox_F.glsl");

    const char* paths[6] = {
      "res/textures/TC_Right.png",
      "res/textures/TC_Left.png",
      "res/textures/TC_Top.png",
      "res/textures/TC_Bottom.png",
      "res/textures/TC_Back.png",
      "res/textures/TC_Front.png",
    };

    textureID = cubemap_create(map, paths, "cubemap_skybox"); 

    int32 texture = 0;
    uniform(state->globalUniforms, "skybox", texture);
    uniform(uniforms, "projection", &camera->projMatrix);
    uniform(uniforms, "view",       &camera->viewMatrix);
  }

  void render(UniformList& globalUniforms) {
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glUseProgram(program); 
    uniform_tick_list(globalUniforms, program); 
    uniform_tick_list(uniforms, program);
  }
};

SkyboxMaterial* material_skybox(AssetMap& map, OpenGLState* state, CameraComponent* c) {
  bool stored;
  return asset_store<SkyboxMaterial>(map, "material_skybox", stored, map, state, c); 
}

