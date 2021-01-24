#pragma once

#include "Asset.cpp"
#include "File.cpp"

struct Texture : public Asset {
  uint32 id;
};

uint32 texture_create(AssetMap& map, const char* path) {
  bool stored;
  Texture* texture = asset_store<Texture>(map, path, stored);
  if(stored) return texture->id;
  texture->id = load_texture(path);
  return texture->id;
}

uint32 cubemap_create(AssetMap& map, const char** paths, const char* name) {
  bool stored;
  Texture* texture = asset_store<Texture>(map, name, stored);
  if(stored) return texture->id;
  texture->id = load_cubemap(paths);
  return texture->id;
}
