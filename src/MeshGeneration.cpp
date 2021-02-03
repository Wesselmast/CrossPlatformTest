#pragma once

#include <vector>

#define SMOOTH_NORMALS 1


inline void generate_index(std::vector<uint32>& indices, uint32 index, int32 depth, int32 res, bool invert = false) {
  if((index + 1) % res && (depth + 1) < res) {
    indices.push_back(index + invert * (res + 1));
    indices.push_back(index + res);
    indices.push_back(index + !invert * (res + 1));
    indices.push_back(index + invert);
    indices.push_back(index + res + 1);
    indices.push_back(index + !invert);
  }
}


static inline void set_triangle_normals(std::vector<float32>& vertices, int32 i1, int32 i2, int32 i3, uint32 layoutSize) {
  i1 *= layoutSize;
  i2 *= layoutSize;
  i3 *= layoutSize;

  Vec3 a = {vertices[i1], vertices[i1 + 1], vertices[i1 + 2]};
  Vec3 b = {vertices[i2], vertices[i2 + 1], vertices[i2 + 2]};
  Vec3 c = {vertices[i3], vertices[i3 + 1], vertices[i3 + 2]};

#if SMOOTH_NORMALS
  Vec3 n = vec3_cross(b - a, c - a);

  vertices[i1 + 3] += n.x; 
  vertices[i1 + 4] += n.y; 
  vertices[i1 + 5] += n.z; 

  vertices[i2 + 3] += n.x; 
  vertices[i2 + 4] += n.y; 
  vertices[i2 + 5] += n.z; 
  
  vertices[i3 + 3] += n.x; 
  vertices[i3 + 4] += n.y; 
  vertices[i3 + 5] += n.z; 
#else
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
#endif
}

static inline void normalize_smooth_normals(std::vector<float32>& vertices, int32 i1, int32 i2, int32 i3, uint32 layoutSize) {
  i1 *= layoutSize;
  i2 *= layoutSize;
  i3 *= layoutSize;

  Vec3 na = vec3_normalize({vertices[i1 + 3], vertices[i1 + 4], vertices[i1 + 5]});
  Vec3 nb = vec3_normalize({vertices[i2 + 3], vertices[i2 + 4], vertices[i2 + 5]});
  Vec3 nc = vec3_normalize({vertices[i3 + 3], vertices[i3 + 4], vertices[i3 + 5]});

  vertices[i1 + 3] = na.x;
  vertices[i1 + 4] = na.y;
  vertices[i1 + 5] = na.z;

  vertices[i2 + 3] = nb.x;
  vertices[i2 + 4] = nb.y;
  vertices[i2 + 5] = nb.z;

  vertices[i3 + 3] = nc.x;
  vertices[i3 + 4] = nc.y;
  vertices[i3 + 5] = nc.z;
}

void generate_normals(std::vector<float32>& vertices, std::vector<uint32>& indices, uint32 layoutSize) {
  for(int32 i = 0; i < indices.size(); i += 3) {
    set_triangle_normals(vertices, indices[i], indices[i + 1], indices[i + 2], layoutSize);
  }

#if SMOOTH_NORMALS
  for(int32 i = 0; i < indices.size(); i += 3) {
    normalize_smooth_normals(vertices, indices[i], indices[i + 1], indices[i + 2], layoutSize);
  }
#endif
}
