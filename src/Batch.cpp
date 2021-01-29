#pragma once

#include <unordered_map>
#include <unordered_set>

struct RendererComponent;
struct Mesh;

struct BatchMap {
  std::unordered_map<Mesh*, std::unordered_set<RendererComponent*>> map;

  void insert(Mesh* key, RendererComponent* value, Mesh* valueMesh) {
    if(valueMesh) map[valueMesh].erase(value);
    map[key].insert(value);
  }

  void remove(Mesh* key, RendererComponent* value) {
    if(key) map[key].erase(value);
  }
};
