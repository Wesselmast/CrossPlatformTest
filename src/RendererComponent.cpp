#pragma once

#include "Components.cpp"
#include "Material.cpp"
#include "Mesh.cpp"
#include "Uniforms.cpp"

#include <forward_list>

struct RendererComponent;
typedef std::forward_list<RendererComponent*> RendererComponentList;

struct RendererComponent : public Component {
  RendererComponentList* list;

  Material* material = nullptr;
  Mesh* mesh;

  uint32 vertexBuffer;
  uint32 indexBuffer;

  inline bool can_render() {
    return material && mesh;
  }

  void render(UniformList& globalUniforms) {
    if(!can_render()) return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    activate_layout(mesh->vertexLayout);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    material->render(globalUniforms);

    glDrawElements(GL_TRIANGLES, mesh->indArrSize / sizeof(uint32), GL_UNSIGNED_INT, (void*)0);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LESS);
    glUseProgram(0);
    glDisableVertexAttribArray(0);
  }

  void set_mesh(Mesh* mesh) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertArrSize, &mesh->vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indArrSize, &mesh->indices[0], GL_STATIC_DRAW);

    this->mesh = mesh;
  }

  void set_material(Material* material) {
    if(this->material) delete this->material;
    this->material = material;
  }

  RendererComponent(RendererComponentList* list) : list(list) {
    list->push_front(this);
  }
  
  RendererComponent(RendererComponentList* list, Mesh* mesh, Material* material) : list(list) {
    set_mesh(mesh);
    set_material(material);
    list->push_front(this);
  }

  ~RendererComponent() {
    if(material) delete material;
    list->remove(this);
  }
};


RendererComponent* component_renderer(RendererComponentList& list, Mesh* mesh, Material* material) {
  return new RendererComponent(&list, mesh, material);
}

RendererComponent* component_renderer(RendererComponentList& list) {
  return new RendererComponent(&list);
}
