#pragma once

#include "Entity.cpp"
#include "File.cpp"

#include <vector>

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

static Entity* skybox(OpenGLState* state, Camera* camera) {
  float32 vertices[] = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
  };

  uint32 indices[] = {
    4, 0, 5,  0, 1, 5,
    6, 2, 4,  2, 0, 4,
    7, 3, 6,  3, 2, 6,
    5, 1, 7,  1, 3, 7,
    0, 2, 1,  2, 3, 1,
    5, 7, 4,  7, 6, 4,
  };

  EntityAttributes attr;
  attr.vertices = vertices;
  attr.vertArrSize = sizeof(vertices);
  attr.vertexCount = 8;
  attr.vertexLayout = LAYOUT_POSITION_3D; 
  attr.vertexShaderPath   = "res/shaders/skybox_V.glsl";
  attr.fragmentShaderPath = "res/shaders/skybox_F.glsl";

  attr.indices = indices;
  attr.indArrSize = sizeof(indices);

  Entity* e = new Entity(state, &attr);

  e->depthFunc = GL_LEQUAL;
  e->textureType = GL_TEXTURE_CUBE_MAP;

  const char* paths[6] = {
    "res/textures/right.png",
    "res/textures/left.png",
    "res/textures/top.png",
    "res/textures/bottom.png",
    "res/textures/back.png",
    "res/textures/front.png",
  };

  e->textureID = load_cubemap(paths); 

  int32 texture = 0;
  uniform(state->globalUniforms, "skybox", texture);
  uniform(e->uniforms, "projection", &camera->projMatrix);
  uniform(e->uniforms, "view", 	     &camera->viewMatrix);

  return e; 
}
