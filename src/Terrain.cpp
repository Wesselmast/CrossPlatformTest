#pragma once

#include "Entity.cpp"
#include "File.cpp"
#include "MeshGeneration.cpp"

static Entity* terrain(OpenGLState* state, const Transform& t, int32 res = 256) {
  int32 w, h;
  uint8* img = load_image("res/textures/depth4.png", &w, &h, 1);

  int32 scale = 20;
  int32 resSqr = res * res;

  std::vector<float32> vertices(resSqr * 6, 0.0f);
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
      v.x -= scale * 0.5f;
      v.z -= scale * 0.5f;
      v.y  = pixel;

      vertices[(i * 6) + 0] = v.x;
      vertices[(i * 6) + 1] = v.y;
      vertices[(i * 6) + 2] = v.z;

      generate_index(indices, i, z, res);
      ++i;
    }
  }

  generate_normals(vertices, indices);

  free_image(img);
  
  EntityAttributes attr;
  attr.vertices = &vertices[0];
  attr.vertArrSize = resSqr * 6 * sizeof(float32);
  attr.vertexCount = resSqr;
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/terrainV.glsl";
  attr.fragmentShaderPath = "res/shaders/terrainF.glsl";

  attr.indices = &indices[0];
  attr.indArrSize = indices.size() * sizeof(uint32);

  Entity* e = new Entity(state, &attr);

  uniform(e->uniforms, "model",     &e->modelMatrix);
  uniform(e->uniforms, "normalMat", &e->normalMatrix);

  return e; 
}

