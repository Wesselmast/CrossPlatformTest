#pragma once

#include "Uniforms.cpp"
#include "File.cpp"
#include "Math.cpp"
#include "Color.cpp"

#include <forward_list>

#define SHADER_VERTEX   GL_VERTEX_SHADER
#define SHADER_FRAGMENT GL_FRAGMENT_SHADER

struct Material {
  std::forward_list<BaseUniform*> uniforms;
  uint32 program;

  const char* shader_type_to_str(int32 type) const {
    switch(type) {
      case SHADER_VERTEX:   return "VERTEX";
      case SHADER_FRAGMENT: return "FRAGMENT";
    }
    return "ERR: Type undefined";
  }

  uint32 load_shader(int32 type, const char* path) {
    uint32 shader = glCreateShader(type);
    std::string sourceStr = load_file_as_string(path);
    const char* source = sourceStr.c_str();

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    int32 result, info;
    glGetShaderiv(shader, GL_COMPILE_STATUS,  &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info);
    if(info > 0) {
      char error[info + 1];
      glGetShaderInfoLog(shader, info, 0, error);
      log_("%s: %s\n", shader_type_to_str(type), error);
    }
    return shader;
  }

  void load_shaders(const char* vPath, const char* fPath) {
    uint32 vertexShader   = load_shader(SHADER_VERTEX,   vPath);
    uint32 fragmentShader = load_shader(SHADER_FRAGMENT, fPath);

    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  virtual void render(OpenGLState* state) {
    glUseProgram(program); 
    uniform_tick_list(state->globalUniforms, program); 
    uniform_tick_list(uniforms, program);
  }

  ~Material() {
    uniform_free_list(uniforms);
  }
};

