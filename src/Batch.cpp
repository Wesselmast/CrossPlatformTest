#pragma once

#include <unordered_map>
#include <unordered_set>

struct TransformComponent;
struct Material;
struct Mesh;

struct Batch {
  Material* material = nullptr;
  Mesh* mesh = nullptr;
  TransformComponent* tc = nullptr;
};

struct BatchMap {
  std::unordered_map<Material*, std::unordered_map<Mesh*, std::unordered_set<TransformComponent*>>> map;

  void insert(Material* nKey1, Mesh* nKey2, Material* oKey1, Mesh* oKey2, TransformComponent* value) {
    if(nKey1 && nKey2 && value) {
      if(oKey1 && oKey2) {
        map[oKey1][oKey2].erase(value);
      }
      map[nKey1][nKey2].insert(value);
    }
  }

  void remove(Batch& b) {
    if(b.material && b.mesh && b.tc) map[b.material][b.mesh].erase(b.tc);
  }
};
