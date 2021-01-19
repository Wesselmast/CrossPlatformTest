#pragma once

#include "Types.cpp"
#include "Math.cpp"
#include "File.cpp"
#include "Color.cpp"
#include "MeshGeneration.cpp"
#include "Layout.cpp"

#include <vector>

//TODO: There should definately be a way to cache these meshes in a nice way

struct Mesh {
  std::vector<float32> vertices;
  uint32 vertexCount; 
  uint32 vertArrSize;
  uint8 vertexLayout;
  
  std::vector<uint32> indices;
  uint32 indArrSize;
};

Mesh* mesh_sphere(int32 res = 50) {
  int32 resSqr = res * res;

  std::vector<float32> vertices(resSqr * 6, 0.0f);
  std::vector<uint32>  indices;

  int32 i = 0;
  for(int32 z = 0; z < res; ++z) {
    float32 angle1 = z * pi() / (res - 1);
    for(int32 x = 0; x < res; ++x) {
      float32 angle2 = x * (pi() * 2) / (res - 1);

      vertices[(i * 6) + 0] = sin(angle1) * cos(angle2);
      vertices[(i * 6) + 1] = cos(angle1);
      vertices[(i * 6) + 2] = sin(angle1) * sin(angle2);

      generate_index(indices, i, z, res, true);
      ++i;
    }
  }

  generate_normals(vertices, indices);

  Mesh* mesh = new Mesh;

  mesh->vertices = vertices;
  mesh->vertArrSize = vertices.size() * sizeof(float32);
  mesh->vertexCount = resSqr;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  mesh->indices = indices;
  mesh->indArrSize = indices.size() * sizeof(uint32);

  return mesh;
}

Mesh* mesh_simple_inverted_cube() {
  Mesh* mesh = new Mesh;

  mesh->vertices = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
  };

  mesh->vertArrSize = mesh->vertices.size() * sizeof(float32);
  mesh->vertexCount = 8;
  mesh->vertexLayout = LAYOUT_POSITION_3D; 

  mesh->indices = {
    4, 0, 5,  0, 1, 5,
    6, 2, 4,  2, 0, 4,
    7, 3, 6,  3, 2, 6,
    5, 1, 7,  1, 3, 7,
    0, 2, 1,  2, 3, 1,
    5, 7, 4,  7, 6, 4,
  };

  mesh->indArrSize = mesh->indices.size() * sizeof(uint32);

  return mesh;
}

Mesh* mesh_terrain(int32 res = 256) {
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
  
  Mesh* mesh = new Mesh;

  mesh->vertices = vertices;
  mesh->vertArrSize = vertices.size() * sizeof(float32);
  mesh->vertexCount = resSqr;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  mesh->indices = indices;
  mesh->indArrSize = indices.size() * sizeof(uint32);

  return mesh;
}

Mesh* mesh_cube() {
  Mesh* mesh = new Mesh;

  mesh->vertices = {
    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,

    // Right face
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

    // Left face
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  };

  mesh->vertArrSize = mesh->vertices.size() * sizeof(float32);
  mesh->vertexCount = 24;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 

  mesh->indices = {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
  };

  mesh->indArrSize = mesh->indices.size() * sizeof(uint32);

  return mesh;
}
