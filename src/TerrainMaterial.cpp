#pragma once

#include "Material.cpp"
#include "Texture.cpp"

struct TerrainMaterial : public Material {
  uint32 mapIDs[3];

  virtual void render(UniformList& globalUniforms) override {
    glUseProgram(program); 

    for(int32 i = 0; i < 3; ++i) {
      glActiveTexture(GL_TEXTURE1 + i);
      glBindTexture(GL_TEXTURE_2D, mapIDs[i]);
    }

    uniform_tick_list(globalUniforms, program); 
    uniform_tick_list(uniforms, program);
  }

  TerrainMaterial(AssetMap& map) {
    load_shaders("res/shaders/terrainV.glsl", "res/shaders/terrainF.glsl");

    mapIDs[0] = texture_create(map, "res/textures/T_Grass.jpg");
    mapIDs[1] = texture_create(map, "res/textures/T_Rocks.jpg");
    mapIDs[2] = texture_create(map, "res/textures/T_Snow.jpg");

    uniform(uniforms, "grassMap",   1);
    uniform(uniforms, "rocksMap",   2);
    uniform(uniforms, "snowMap",    3);
  }
};

TerrainMaterial* material_terrain(AssetMap& map) {
  bool stored;
  return asset_store<TerrainMaterial>(map, "material_terrain", stored, map); 
}
