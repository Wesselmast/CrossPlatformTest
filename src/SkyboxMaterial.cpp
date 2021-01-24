#pragma once

#include "Material.cpp"
#include "CameraComponent.cpp"
#include "Texture.cpp"

struct SkyboxMaterial : public Material {
  uint32 textureID;

  SkyboxMaterial(OpenGLState* state, CameraComponent* camera, AssetMap& map) {
    load_shaders("res/shaders/skybox_V.glsl", "res/shaders/skybox_F.glsl");

    const char* paths[6] = {
      "res/textures/right.png",
      "res/textures/left.png",
      "res/textures/top.png",
      "res/textures/bottom.png",
      "res/textures/back.png",
      "res/textures/front.png",
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

SkyboxMaterial* material_skybox(OpenGLState* state, CameraComponent* c, AssetMap& map) {
  return new SkyboxMaterial(state, c, map);
}

