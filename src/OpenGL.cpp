#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "List.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

struct Entity;
struct Camera;

struct OpenGLState {
  List<Entity*> entities;
  Camera* camera;
  uint16 windowWidth;
  uint16 windowHeight;
};

#include "Entity.cpp"
#include "Camera.cpp"

OpenGLState* gl_start() {
  if(glewInit() != GLEW_OK) {
    printf("glew is bogged!");
    return nullptr;
  }

  OpenGLState* state = (OpenGLState*)malloc(sizeof(OpenGLState));
  state->camera = create_camera(state);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

 // glClearDepth(1.0f);
 // glEnable(GL_DEPTH_TEST);
 // glDepthFunc(GL_LEQUAL);
 // glShadeModel(GL_SMOOTH);
 // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  return state;
}

void gl_render(OpenGLState* state) {
  glClear(GL_COLOR_BUFFER_BIT);

  uint16& w = state->windowWidth;
  uint16& h = state->windowHeight;
  glViewport(0, 0, w, h);
  
  Mat4 proj = state->camera->get_view_projection();
  Node<Entity*>* current = state->entities.head;
  while(current) {
    Entity* e = current->data;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, e->vertexBuffer);
    glVertexAttribPointer(0, e->vertexSize, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glUseProgram(e->program);

    float32& r = e->color.x;
    float32& g = e->color.y;
    float32& b = e->color.z;

    glUniform3f(glGetUniformLocation(e->program, "objColor"), r, g, b);
    glUniformMatrix4fv(glGetUniformLocation(e->program, "projection"), 1, GL_FALSE, &(proj.m0[0]));
    glDrawArrays(GL_TRIANGLES, 0, e->vertexCount);

    glUseProgram(0);
    glDisableVertexAttribArray(0);

    current = current->next;
  }

  glFlush();
}

void gl_end(OpenGLState* state) {
  //later add in checking for level saving
  state->entities.free_list();
  free(state->camera);
  free(state);
}

