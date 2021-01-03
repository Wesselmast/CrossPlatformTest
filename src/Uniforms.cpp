#pragma once

#include "Types.cpp"
#include "Math.cpp"
#include "List.cpp"
#include "Color.cpp"

#include <string.h>

typedef void(*fptr_uniformtick)(int32, void*);

struct Uniform {
  char name[64];
  fptr_uniformtick tick;
  void* data;
  uint8 oneTime;
};

inline void uniform_tick_vec3(int32 location, void* data) {
  Vec3* v = (Vec3*)data;
  glUniform3f(location, v->x, v->y, v->z);
}

inline void uniform_tick_color(int32 location, void* data) {
  Color* c = (Color*)data;
  glUniform3f(location, c->r, c->g, c->b);
}

inline void uniform_tick_mat4(int32 location, void* data) {
  glUniformMatrix4fv(location, 1, GL_TRUE, (float32*)data); 
}

Uniform* uniform_create_generic(const char* name, void* data, uint32 dataSize, bool oneTime, fptr_uniformtick tick) {
  Uniform* uniform = (Uniform*)malloc(sizeof(Uniform));

  strcpy(uniform->name, name);
  uniform->tick = tick;
  uniform->oneTime = oneTime;

  if(oneTime) {
    uniform->data = malloc(dataSize);
    memcpy(uniform->data, data, dataSize);
  }
  else uniform->data = data;
  
  return uniform;
}

Uniform* uniform_create_color(const char* name, Color* data, bool oneTime = false) {
  return uniform_create_generic(name, (void*)data, sizeof(Color), oneTime, &uniform_tick_color);
}

Uniform* uniform_create_vec3(const char* name, Vec3* data, bool oneTime = false) {
  return uniform_create_generic(name, (void*)data, sizeof(Vec3), oneTime, &uniform_tick_vec3);
}

Uniform* uniform_create_mat4(const char* name, Mat4* data, bool oneTime = false) {
  return uniform_create_generic(name, (void*)data, sizeof(Mat4), oneTime, &uniform_tick_mat4);
}

inline void uniform_tick_list(List<Uniform*>* uniforms, int32 program) {
  Node<Uniform*>* current = uniforms->head;
  int32 len = uniforms->length;
  for(int32 i = 0; i < len; ++i) {
    Uniform* uniform = current->data;
    uniform->tick(glGetUniformLocation(program, uniform->name), uniform->data);
    current = current->next;
  }
}

void uniform_free_list(List<Uniform*>* uniforms) {
  Node<Uniform*>* current = uniforms->head;
  Node<Uniform*>* next = nullptr;
  int32 len = uniforms->length;

  for(int32 i = 0; i < len; ++i) {
    next = current->next;
    Uniform* uniform = current->data;
    if(uniform->oneTime) free(uniform->data);
    free(current);
    current = next;
  }

  free(uniforms);
}
