#pragma once

#include "Material.cpp"
#include "File.cpp"

uint32 load_cubemap(const char** faces) {
  uint32 textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int32 w, h;
  for(uint32 i = 0; i < 6; ++i) {
    uint8* data = load_image(faces[i], &w, &h, 3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free_image(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

struct SkyboxMaterial : public Material {
  uint32 textureID;

  SkyboxMaterial(OpenGLState* state, Camera* camera) {
    load_shaders("res/shaders/skybox_V.glsl", "res/shaders/skybox_F.glsl");

    const char* paths[6] = {
      "res/textures/right.png",
      "res/textures/left.png",
      "res/textures/top.png",
      "res/textures/bottom.png",
      "res/textures/back.png",
      "res/textures/front.png",
    };

    textureID = load_cubemap(paths); 

    int32 texture = 0;
    uniform(state->globalUniforms, "skybox", texture);
    uniform(uniforms, "projection", &camera->projMatrix);
    uniform(uniforms, "view",       &camera->viewMatrix);
  }

  void render(OpenGLState* state) override {
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glUseProgram(program); 
    uniform_tick_list(state->globalUniforms, program); 
    uniform_tick_list(uniforms, program);
  }
};

SkyboxMaterial* material_skybox(OpenGLState* state, Camera* c) {
  return new SkyboxMaterial(state, c);
}

