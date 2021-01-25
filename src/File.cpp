#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include "Types.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

std::string load_file_as_string(const char* path) {
  std::ifstream stream(path, std::ios::in);

  if(!stream.is_open()) {
    printf("ERROR");
    return "ERROR";
  }

  std::stringstream sstream;
  sstream << stream.rdbuf();
  stream.close();

  return sstream.str();
}

//wrappers for stb, for if I ever want to remove it

uint8* load_image(const char* path, int32* width, int32* height, int32 channels) {
  int32 c;
  return stbi_load(path, width, height, &c, channels); 
}

bool write_image(const char* path, int32 width, int32 height, int32 channels, uint8* data) {
  return stbi_write_png(path, width, height, channels, data, 0);
}

void free_image(uint8* img) {
  stbi_image_free(img);
}

//end 

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

uint32 load_texture(const char* path) {
  uint32 textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  int32 w, h;
  uint8* data = load_image(path, &w, &h, 3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  free_image(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return textureID;
}
