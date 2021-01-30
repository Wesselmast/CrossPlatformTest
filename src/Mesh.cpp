#pragma once

#include "Types.cpp"
#include "Math.cpp"
#include "Asset.cpp"
#include "File.cpp"
#include "Color.cpp"
#include "MeshGeneration.cpp"
#include "Perlin.cpp"
#include "Layout.cpp"

#include <vector>

struct Mesh : public Asset {
  std::vector<float32> vertices;
  uint32 vertexCount; 
  uint32 vertArrSize;
  uint8 vertexLayout;
  
  std::vector<uint32> indices;
  uint32 indArrSize;
  uint32 runTimeArrSize;

  uint32 vertexBuffer;
  uint32 indexBuffer;

  void bind_vb(bool dynamic = false) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertArrSize, &vertices[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
  }

  void bind_ib(bool dynamic = false) {
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indArrSize, &indices[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
  }

  Mesh* init(bool dynamic = false) {
    bind_vb(dynamic);
    bind_ib(dynamic);

    runTimeArrSize = indArrSize / sizeof(uint32);

    return this;
  }

  void prepare_draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    activate_layout(vertexLayout);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  }

  void draw() {
    glDrawElements(GL_TRIANGLES, runTimeArrSize, GL_UNSIGNED_INT, (void*)0);
  }
};

Mesh* mesh_sphere(AssetMap& map, int32 res = 50, const char* name = "mesh_sphere") {
  bool stored;
  Mesh* mesh = asset_store<Mesh>(map, name, stored);
  if(stored) return mesh;

  int32 resSqr = res * res;

  std::vector<float32> vertices(resSqr * 8, 0.0f);
  std::vector<uint32>  indices;

  int32 i = 0;
  for(int32 z = 0; z < res; ++z) {
    float32 angle1 = z * pi() / (res - 1);
    for(int32 x = 0; x < res; ++x) {
      float32 angle2 = x * (pi() * 2) / (res - 1);

      vertices[(i * 8) + 0] = sin(angle1) * cos(angle2);
      vertices[(i * 8) + 1] = cos(angle1);
      vertices[(i * 8) + 2] = sin(angle1) * sin(angle2);
      vertices[(i * 8) + 6] = vertices[(i * 8) + 0];
      vertices[(i * 8) + 7] = vertices[(i * 8) + 1];

      generate_index(indices, i, z, res, true);
      ++i;
    }
  }

  generate_normals(vertices, indices, 8);

  mesh->vertices = vertices;
  mesh->vertArrSize = vertices.size() * sizeof(float32);
  mesh->vertexCount = resSqr;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL | LAYOUT_TEXCOORD; 
  mesh->indices = indices;
  mesh->indArrSize = indices.size() * sizeof(uint32);

  return mesh->init();
}

Mesh* mesh_simple_inverted_cube(AssetMap& map) {
  bool stored;
  Mesh* mesh = asset_store<Mesh>(map, "mesh_simple_inverted_cube", stored);
  if(stored) return mesh;

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

  return mesh->init();
}

Mesh* mesh_terrain(AssetMap& map, int32 res = 256, const char* name = "mesh_terrain") {
  bool stored;
  Mesh* mesh = asset_store<Mesh>(map, name, stored);
  if(stored) return mesh;

  int32 scale = 20;
  int32 resSqr = res * res;

  int32 freq = 8;
  float64 period = (float64)freq / (float64)res;

  std::vector<float32> vertices(resSqr * 8, 0.0f);
  std::vector<uint32>  indices;

  perlin_seed = rand();

  int32 i = 0;
  for(int32 z = 0; z < res; ++z) {
    for(int32 x = 0; x < res; ++x) {
      Vec3 v = zero();
      v.x = (float32)x / (float32)res;
      v.z = (float32)z / (float32)res;

      float32 pixel = fractal_noise(x * period, z * period, 6, 0.4);

      v.x *= scale;
      v.z *= scale;
      v.x -= scale * 0.5f;
      v.z -= scale * 0.5f;
      v.y  = pixel;

      vertices[(i * 8) + 0] = v.x;
      vertices[(i * 8) + 1] = v.y;
      vertices[(i * 8) + 2] = v.z;
      vertices[(i * 8) + 6] = vertices[(i * 8) + 0];
      vertices[(i * 8) + 7] = vertices[(i * 8) + 2];

      generate_index(indices, i, z, res);
      ++i;
    }
  }

  generate_normals(vertices, indices, 8);

  mesh->vertices = vertices;
  mesh->vertArrSize = vertices.size() * sizeof(float32);
  mesh->vertexCount = resSqr;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL | LAYOUT_TEXCOORD; 
  mesh->indices = indices;
  mesh->indArrSize = indices.size() * sizeof(uint32);

  return mesh->init(true);
}

Mesh* mesh_cube(AssetMap& map) {
  bool stored;
  Mesh* mesh = asset_store<Mesh>(map, "mesh_cube", stored);
  if(stored) return mesh;

  mesh->vertices = {
    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    // Right face
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

    // Left face
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  };

  mesh->vertArrSize = mesh->vertices.size() * sizeof(float32);
  mesh->vertexCount = 24;
  mesh->vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL | LAYOUT_TEXCOORD; 

  mesh->indices = {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
  };

  mesh->indArrSize = mesh->indices.size() * sizeof(uint32);

  return mesh->init();
}
