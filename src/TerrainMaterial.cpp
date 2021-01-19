#pragma once

#include "Material.cpp"
#include "TransformComponent.cpp"

struct TerrainMaterial : public Material {
  Mat4* modelMatrix;
  Mat4* normalMatrix;

  TerrainMaterial(TransformComponent* t) {
    modelMatrix = &t->modelMatrix;
    normalMatrix = &t->normalMatrix;

    load_shaders("res/shaders/terrainV.glsl", "res/shaders/terrainF.glsl");

    uniform(uniforms, "model",     modelMatrix);
    uniform(uniforms, "normalMat", normalMatrix);
  }
};

TerrainMaterial* material_terrain(TransformComponent* t) {
  return new TerrainMaterial(t);  
}
