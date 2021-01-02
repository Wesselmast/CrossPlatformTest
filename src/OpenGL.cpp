#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "List.cpp"
#include "Color.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Uniforms.cpp"

#define LAYOUT_POSITION_3D 0x01
#define LAYOUT_POSITION_2D 0x02
#define LAYOUT_NORMAL      0x04
#define LAYOUT_TEXCOORD    0x08
#define LAYOUT_COLOR       0x10

inline int32 get_layout_size(uint8 layout) {
  int32 size = 0;

  size += bool(layout & LAYOUT_POSITION_3D) * 3;
  size += bool(layout & LAYOUT_POSITION_2D) * 2;
  size += bool(layout & LAYOUT_NORMAL)      * 3;
  size += bool(layout & LAYOUT_TEXCOORD)    * 2;
  size += bool(layout & LAYOUT_COLOR)       * 3;

  return size * sizeof(float32);
}

inline void activate_layout_element(uint8& pos, int64& stride, uint8 totalSize, uint8 elementSize) {
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, elementSize, GL_FLOAT, GL_FALSE, totalSize, (void*)stride);
  stride += elementSize * sizeof(float32);
  ++pos;
} 

inline void activate_layout(uint8 layout) {
  int32 size = get_layout_size(layout);
  uint8 pos = 0;
  int64 stride = 0;

  if(layout & LAYOUT_POSITION_3D) activate_layout_element(pos, stride, size, 3);
  if(layout & LAYOUT_POSITION_2D) activate_layout_element(pos, stride, size, 2);
  if(layout & LAYOUT_NORMAL)      activate_layout_element(pos, stride, size, 3);
  if(layout & LAYOUT_TEXCOORD)    activate_layout_element(pos, stride, size, 2);
  if(layout & LAYOUT_COLOR)       activate_layout_element(pos, stride, size, 3);
}

struct Entity;

struct OpenGLState {
  List<Entity*> entities;
  List<Uniform*>* globalUniforms;

  Mat4 vp;
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

inline void set_background_color(int32 hex) {
  Color c = hex_to_color(hex);
  glClearColor(c.r, c.g, c.b, 1.0f);
} 

OpenGLState* gl_start() {
  if(glewInit() != GLEW_OK) {
    printf("glew is bogged!");
    return nullptr;
  }

  glClearDepth(1.0f);
  set_background_color(0x2B2D42);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  OpenGLState* state = (OpenGLState*)malloc(sizeof(OpenGLState));
  state->globalUniforms = (List<Uniform*>*)malloc(sizeof(List<Uniform*>));
  state->globalUniforms->insert(uniform_create_mat4("viewProj", &state->vp));

  return state;
}

void gl_tick(OpenGLState* state, Camera* c) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Node<Entity*>* current = state->entities.head;
  while(current) {
    Entity* e = current->data;
    
    glBindBuffer(GL_ARRAY_BUFFER, e->vertexBuffer);
    activate_layout(e->vertexLayout);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e->indexBuffer);

    uint32& program = e->program;
    glUseProgram(program);

    uniform_tick_list(state->globalUniforms, program); 
    uniform_tick_list(e->uniforms, program);

    glDrawElements(GL_TRIANGLES, e->indexBufferSize, GL_UNSIGNED_INT, (void*)0);

    glUseProgram(0);
    glDisableVertexAttribArray(0);

    current = current->next;
  }
}

void gl_end(OpenGLState* state) {
  //later add in checking for level saving

  Node<Entity*>* current = state->entities.head;
  while(current) {
    destroy_entity_partial(current->data);
    current = current->next;
  }

  state->entities.free_list();
  uniform_free_list(state->globalUniforms);
  free(state);
}

void gl_resize(OpenGLState* state, uint16 w, uint16 h) {
  state->windowWidth = w;
  state->windowHeight = h;
  glViewport(0, 0, w, h);
} 
