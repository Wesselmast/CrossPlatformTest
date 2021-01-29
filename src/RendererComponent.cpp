#pragma once

#include "Batch.cpp"
#include "Components.cpp"
#include "Material.cpp"
#include "Mesh.cpp"
#include "Uniforms.cpp"

struct RendererComponent : public Component {
  BatchMap* map;

  Material* material = nullptr;
  Mesh* mesh = nullptr;

  inline bool can_render() {
    return material && mesh;
  }

  void render(UniformList& globalUniforms) {
    if(!can_render()) return;

    material->render(globalUniforms);
    glDrawElements(GL_TRIANGLES, mesh->runTimeArrSize, GL_UNSIGNED_INT, (void*)0);
  }

  void set_mesh(Mesh* key) {
    map->insert(key, this, this->mesh);
    this->mesh = key;
  }

  void set_material(Material* material) {
    if(this->material) delete this->material;
    this->material = material;
  }

  RendererComponent(BatchMap* map) : map(map) {
  }
  
  RendererComponent(BatchMap* map, Mesh* mesh, Material* material) : map(map) {
    set_mesh(mesh);
    set_material(material);
  }

  ~RendererComponent() {
    if(material) delete material;
    map->remove(this->mesh, this);
  }
};


RendererComponent* component_renderer(BatchMap& map, Mesh* mesh, Material* material) {
  return new RendererComponent(&map, mesh, material);
}

RendererComponent* component_renderer(BatchMap& map) {
  return new RendererComponent(&map);
}
