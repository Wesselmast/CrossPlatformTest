#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include "Types.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

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

void free_image(uint8* img) {
  stbi_image_free(img);
}
