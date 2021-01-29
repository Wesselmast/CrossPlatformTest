#pragma once

#include "Batch.cpp"
#include "Components.cpp"
#include "Material.cpp"
#include "Mesh.cpp"
#include "TransformComponent.cpp"
#include "Uniforms.cpp"

struct RendererComponent : public Component {
  BatchMap* map;
  Batch batch;

  void set_material(Material* material) {
    map->insert(material, batch.mesh, batch.material, batch.mesh, batch.tc);
    batch.material = material;
  }

  void set_mesh(Mesh* mesh) {
    map->insert(batch.material, mesh, batch.material, batch.mesh, batch.tc);
    batch.mesh = mesh;
  }


  void set_transform_component(TransformComponent* tc) {
    map->insert(batch.material, batch.mesh, batch.material, batch.mesh, tc);
    batch.tc = tc;
  }

  RendererComponent(BatchMap* map) : map(map) {
  }
  
  RendererComponent(BatchMap* map, Batch& b) : map(map) {
    set_mesh(b.mesh);
    set_material(b.material);
    set_transform_component(b.tc);
  }

  ~RendererComponent() {
    map->remove(batch);
  }
};

RendererComponent* component_renderer(BatchMap& map, Batch& batch) {
  return new RendererComponent(&map, batch);
}

RendererComponent* component_renderer(BatchMap& map) {
  return new RendererComponent(&map);
}
