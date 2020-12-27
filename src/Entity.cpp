#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "File.cpp"

struct EntityAttributes {
  float32* vertices;
  uint32 size;
  Vec3 color;
  uint32 vertexCount;
  uint32 vertexSize;
};

struct Entity {
  uint32 program;
  uint32 vertexBuffer;
  uint32 vertexCount;
  uint32 vertexSize;
  Vec3 color;
  
  void load_shaders(const char* vPath, const char* fPath) {
    uint32 vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    uint32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();

    int32 result;
    int32 info;

    std::string vSourceStr = load_file_as_string(vPath);
    std::string fSourceStr = load_file_as_string(fPath);

    const char* vertexSource = vSourceStr.c_str();
    const char* fragmentSource = fSourceStr.c_str();

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &info);
    if(info > 0) {
      char error[info + 1];
      glGetShaderInfoLog(vertexShader, info, NULL, error);
      log_("VERTEX: %s\n", error);
    }
    
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &info);
    if(info > 0) {
      char error[info + 1];
      glGetShaderInfoLog(fragmentShader, info, NULL, error);
      log_("FRAGMENT: %s\n", error);
    }  

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  Entity* init(OpenGLState* state, EntityAttributes* attr) {
    color = attr->color;
    vertexCount = attr->vertexCount;
    vertexSize = attr->vertexSize;

    load_shaders("res/shaders/defaultV.glsl", "res/shaders/defaultF.glsl");
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, attr->size, attr->vertices, GL_STATIC_DRAW);

    state->entities.insert(this);
    return this;
  }
};

Entity* rect(OpenGLState* state, const Vec3& position, float32 scale, const Vec3& color) {
  const float32& x = position.x;
  const float32& y = position.y;
  const float32& z = position.z;

  float32 vertices[] = {
    x - scale, y - scale, z, 
    x + scale, y - scale, z,
    x + scale, y + scale, z,
    x - scale, y - scale, z,
    x - scale, y + scale, z,
    x + scale, y + scale, z,
  };

  EntityAttributes attr;
  attr.vertices = vertices;
  attr.size = sizeof(vertices);
  attr.vertexCount = 6;
  attr.vertexSize = 3;
  attr.color = color;

  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}


