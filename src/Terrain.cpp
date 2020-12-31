#pragma once

#include "Entity.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <vector>

void set_triangle_normals(std::vector<float32>& vertices, int32 i1, int32 i2, int32 i3) {
  i1 *= 6;
  i2 *= 6;
  i3 *= 6;

  Vec3 a = {vertices[i1], vertices[i1 + 1], vertices[i1 + 2]};
  Vec3 b = {vertices[i2], vertices[i2 + 1], vertices[i2 + 2]};
  Vec3 c = {vertices[i3], vertices[i3 + 1], vertices[i3 + 2]};

  Vec3 n = vec3_normalize(vec3_cross(b - a, c - a));

  vertices[i1 + 3] = n.x; 
  vertices[i1 + 4] = n.y; 
  vertices[i1 + 5] = n.z; 

  vertices[i2 + 3] = n.x; 
  vertices[i2 + 4] = n.y; 
  vertices[i2 + 5] = n.z; 
  
  vertices[i3 + 3] = n.x; 
  vertices[i3 + 4] = n.y; 
  vertices[i3 + 5] = n.z; 
}

Entity* terrain(OpenGLState* state, const Transform& t, int32 hexColor, int32 res = 256, float32 height = 3.0f) {
  int32 w, h, c;
  uint8* img = stbi_load("res/textures/depth.png", &w, &h, &c, 1);

  int32 scale = 20;
  int32 resSqr = res * res;

  std::vector<float32> vertices(resSqr * 6);
  std::vector<uint32>  indices;

  int32 i = 0;
  for(int32 z = 0; z < res; ++z) {
    for(int32 x = 0; x < res; ++x) {
      Vec3 v = zero();
      v.x = (float32)x / (float32)res;
      v.z = (float32)z / (float32)res;

      int32 ix = round(v.x * w);
      int32 iy = round(v.z * h);
      float32 pixel = (img + (ix + iy * h))[0] / 255.0f;

      v.x *= scale;
      v.z *= scale;
      v.x -= scale / 2;
      v.z -= scale / 2;
      v.y  = pixel * height;

      vertices[(i * 6) + 0] = v.x;
      vertices[(i * 6) + 1] = v.y;
      vertices[(i * 6) + 2] = v.z;

      if((i + 1) % res && (z + 1) < res) {
	indices.push_back(i);
	indices.push_back(i + res);
	indices.push_back(i + res + 1);
	indices.push_back(i);
	indices.push_back(i + res + 1);
	indices.push_back(i + 1);
      }
      ++i;
    }
  }

  for(int32 i = 0; i < indices.size(); i += 3) {
    set_triangle_normals(vertices, indices[i], indices[i + 1], indices[i + 2]);
  }

  stbi_image_free(img);
  
  EntityAttributes attr;
  attr.vertices = &vertices[0];
  attr.vertArrSize = resSqr * 6 * sizeof(float32);
  attr.vertexCount = resSqr;
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/defaultV.glsl";
  attr.fragmentShaderPath = "res/shaders/defaultF.glsl";

  attr.indices = &indices[0];
  attr.indArrSize = indices.size() * sizeof(uint32);

  attr.hexColor = hexColor;
  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}

