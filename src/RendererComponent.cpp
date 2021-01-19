#pragma once

#include "Components.cpp"
#include "Material.cpp"
#include "Mesh.cpp"

//TODO: the meshes and materials currently leak because they are being shared between actors

struct RendererComponent : public Component {
  Material* mat;
  Mesh* mesh;

  uint32 vertexBuffer;
  uint32 indexBuffer;

  void tick(OpenGLState* state) override {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    activate_layout(mesh->vertexLayout);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    mat->render(state);

    glDrawElements(GL_TRIANGLES, mesh->indArrSize / sizeof(uint32), GL_UNSIGNED_INT, (void*)0);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LESS);
    glUseProgram(0);
    glDisableVertexAttribArray(0);
  }
  
  RendererComponent(Mesh* mesh, Material* mat) : mesh(mesh), mat(mat) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertArrSize, &mesh->vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indArrSize, &mesh->indices[0], GL_STATIC_DRAW);
  }

  ~RendererComponent() {
  }
};

RendererComponent* component_renderer(Mesh* mesh, Material* material) {
  return new RendererComponent(mesh, material);
}
