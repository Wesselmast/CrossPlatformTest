#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "List.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

struct Entity;

struct OpenGLState {
  List<Entity*> entities;
  uint16 windowWidth;
  uint16 windowHeight;
};

#include "Entity.cpp"
#include "Camera.cpp"

#define RENDERMODE_NORMAL     0x01
#define RENDERMODE_WIREFRAME  0x02
#define RENDERMODE_POINT      0x04

inline void set_rendermode(uint8 renderMode) {
  switch(renderMode) {
    case RENDERMODE_NORMAL:    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
    case RENDERMODE_WIREFRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
    case RENDERMODE_POINT:     glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
  }
}

OpenGLState* gl_start() {
  if(glewInit() != GLEW_OK) {
    printf("glew is bogged!");
    return nullptr;
  }

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  return (OpenGLState*)malloc(sizeof(OpenGLState));
}

void gl_tick(OpenGLState* state, Camera* c) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  uint16& w = state->windowWidth;
  uint16& h = state->windowHeight;
  glViewport(0, 0, w, h);
  
  Mat4 vp = c->get_view_projection();
  Node<Entity*>* current = state->entities.head;
  while(current) {
    Entity* e = current->data;

    glBindBuffer(GL_ARRAY_BUFFER, e->vertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void*)12);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e->indexBuffer);

    glUseProgram(e->program);

    float32& r = e->color.x;
    float32& g = e->color.y;
    float32& b = e->color.z;

    glUniform3f(glGetUniformLocation(e->program, "objColor"), r, g, b);
    glUniformMatrix4fv(glGetUniformLocation(e->program, "model"),    1, GL_TRUE, &(e->modelMatrix.m0[0]));
    glUniformMatrix4fv(glGetUniformLocation(e->program, "viewProj"), 1, GL_TRUE, &(vp.m0[0]));

    glDrawElements(GL_TRIANGLES, e->indexBufferSize, GL_UNSIGNED_INT, (void*)0);

    glUseProgram(0);
    glDisableVertexAttribArray(0);

    current = current->next;
  }

  glFlush();
}

void gl_end(OpenGLState* state) {
  //later add in checking for level saving
  state->entities.free_list();
  free(state);
}

