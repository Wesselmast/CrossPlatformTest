#pragma once

#include "Entity.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <vector>

Entity* terrain(OpenGLState* state, const Transform& t, int32 hexColor) {
  //Entity* entity = rect(state, t, hexColor);

  // START LOADING THE IMAGE HERE

  int32 w, h, c;
  uint8* img = stbi_load("res/textures/depth.png", &w, &h, &c, 1);

  int32 tRes = 128;
  int32 tScale = 20;
  int32 tHeight = 3;
  int32 tResSqr = tRes * tRes;

  std::vector<float32> vertices(tResSqr * 6);
  std::vector<uint32>  indices;

  int32 i = 0;
  for(int32 z = 0; z < tRes; ++z) {
    for(int32 x = 0; x < tRes; ++x) {
      Vec3 v = zero();
      v.x = (float32)x / (float32)tRes;
      v.z = (float32)z / (float32)tRes;

      int32 ix = round(v.x * w);
      int32 iy = round(v.z * h);
      float32 pixel = (img + (ix + iy * h))[0] / 255.0f;

      v.x *= tScale;
      v.z *= tScale;
      v.x -= tScale / 2;
      v.z -= tScale / 2;
      v.y *= pixel * tHeight;

      vertices[(i * 6) + 0] = v.x;
      vertices[(i * 6) + 1] = v.y;
      vertices[(i * 6) + 2] = v.z;
      vertices[(i * 6) + 3] = 0.0f;
      vertices[(i * 6) + 4] = 1.0f;
      vertices[(i * 6) + 5] = 0.0f;

      if((i + 1) % tRes && (z + 1) < tRes) {
	indices.push_back(i);
	indices.push_back(i + tRes);
	indices.push_back(i + tRes + 1);
	indices.push_back(i);
	indices.push_back(i + tRes + 1);
	indices.push_back(i + 1);
      }
      ++i;
    }
  }

  stbi_image_free(img);
  
  EntityAttributes attr;
  attr.vertices = &vertices[0];
  attr.vertArrSize = tResSqr * 6 * sizeof(float32);
  attr.vertexCount = tResSqr;
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/defaultV.glsl";
  attr.fragmentShaderPath = "res/shaders/defaultF.glsl";

  attr.indices = &indices[0];
  attr.indArrSize = indices.size() * sizeof(uint32);

  attr.hexColor = hexColor;
  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}

//  Vec3 a = { -1.0f, -1.0f,  1.0f };
//  Vec3 b = {  1.0f, -1.0f,  1.0f };
//  Vec3 c = {  1.0f,  1.0f,  1.0f };
//
//  Vec3 v = vec3_normalize(vec3_cross(b - a, c - a));
