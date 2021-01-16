#pragma once

#include "Types.cpp"
#include "Math.cpp"
#include "List.cpp"
#include "Color.cpp"

#include <string>
#include <cstring>
#include <forward_list>

typedef void(*fptr_uniformtick)(int32, void*);

template<typename T> 
inline void uniform_tick(int32 location, T data) {
  printf("ERR: Uniform not implemented\n");
}

template<>
inline void uniform_tick(int32 location, float32 data) {
  glUniform1f(location, data);
}

template<>
inline void uniform_tick(int32 location, float32* data) {
  glUniform1f(location, *data);
}

template<>
inline void uniform_tick(int32 location, int32 data) {
  glUniform1i(location, data);
}

template<>
inline void uniform_tick(int32 location, int32* data) {
  glUniform1i(location, *data);
}

template<>
inline void uniform_tick(int32 location, Vec3 data) {
  glUniform3f(location, data.x, data.y, data.z);
}

template<>
inline void uniform_tick(int32 location, Vec3* data) {
  glUniform3f(location, data->x, data->y, data->z);
}

template<>
inline void uniform_tick(int32 location, Color data) {
  glUniform3f(location, data.r, data.g, data.b);
}

template<>
inline void uniform_tick(int32 location, Color* data) {
  glUniform3f(location, data->r, data->g, data->b);
}

template<>
inline void uniform_tick(int32 location, Mat4 data) {
  glUniformMatrix4fv(location, 1, GL_TRUE, (float32*)&data.m0[0]); 
}

template<>
inline void uniform_tick(int32 location, Mat4* data) {
  glUniformMatrix4fv(location, 1, GL_TRUE, (float32*)&data->m0[0]); 
}

struct BaseUniform {
  virtual void tick(int32 program) = 0;
  virtual void print_name() = 0;
};

template<typename T>
struct Uniform : public BaseUniform {
  Uniform(const std::string& name, T data) : data(data), name(name) {
  }

  inline void tick(int32 program) override {
    int32 location = glGetUniformLocation(program, name.c_str());
    uniform_tick(location, data);
  }

  inline void print_name() override {
    printf("%s\n", name.c_str());
  }

  std::string name;
  T data;
};


template<typename T>
void uniform(std::forward_list<BaseUniform*>& uniforms, const std::string& name, T data) {
  uniforms.emplace_front(new Uniform<T>(name, data));
}

inline void uniform_tick_list(std::forward_list<BaseUniform*>& uniforms, int32 program) {
  for(BaseUniform* u : uniforms) {
    u->tick(program);
  }
}

void uniform_free_list(std::forward_list<BaseUniform*>& uniforms) {
  while(!uniforms.empty()) {
    delete uniforms.front();
    uniforms.pop_front();
  }
}
