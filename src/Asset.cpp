#pragma once
#include <unordered_map>

//TODO: Make sure assets get destroyed at the end

struct Asset {
  virtual ~Asset() {}
};

typedef std::unordered_map<const char*, Asset*> AssetMap;

template<typename T, typename... Args>
T* asset_store(AssetMap& map, const char* name, bool& stored, Args... args) {
  stored = map.find(name) != map.end();
  if(stored) {
    return (T*)map[name];
  }

  T* asset = new T(args...);
  map[name] = asset;
  return asset;
}
