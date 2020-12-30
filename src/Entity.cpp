#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "File.cpp"
#include "Color.cpp"

struct EntityAttributes {
  float32* vertices;
  uint32 vertexCount; 
  uint32 vertexSize;
  uint32 vertArrSize;
  
  uint32* indices;
  uint32 indArrSize;

  int32 hexColor;
  Transform transform;
};

struct Entity {
  uint32 program;
  
  uint32 vertexBuffer;
  uint32 vertexCount;
  uint32 vertexSize;

  uint32 indexBuffer;
  uint32 indexBufferSize;  

  Color color;
  Mat4 modelMatrix;
  Transform transform;
  
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

  void set_scale(const Vec3& scale) {
    transform.scale = scale;
    set_transform(transform);
  }

  void set_rotation(const Vec3& rotation) {
    transform.rotation = rotation;
    set_transform(transform);
  }

  void set_position(const Vec3& position) {
    transform.position = position;
    set_transform(transform);
  }

  void set_transform(const Transform& t) {
    modelMatrix = mat4_scaling(t.scale) * mat4_euler_rotation(t.rotation) * mat4_translation(t.position);
    this->transform = t;
  }

  Entity* init(OpenGLState* state, EntityAttributes* attr) {
    color = hex_to_color(attr->hexColor);
    vertexCount = attr->vertexCount;
    vertexSize = attr->vertexSize;
    indexBufferSize = attr->indArrSize / sizeof(uint32);

    load_shaders("res/shaders/defaultV.glsl", "res/shaders/defaultF.glsl");

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, attr->vertArrSize, attr->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, attr->indArrSize, attr->indices, GL_STATIC_DRAW);

    transform = attr->transform;
    set_transform(transform);

    state->entities.insert(this);
    return this;
  }
};

Entity* rect(OpenGLState* state, const Transform& t, int32 hexColor) {
  float32 vertices[] = {
    -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
    -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
  };

  uint32 indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  EntityAttributes attr;
  attr.vertices = vertices;
  attr.vertArrSize = sizeof(vertices);
  attr.vertexCount = 4;
  attr.vertexSize = 3; //irr right now
  attr.transform = t;

  attr.indices = indices;
  attr.indArrSize = sizeof(indices);

  attr.hexColor = hexColor;
  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}

Entity* cube(OpenGLState* state, const Transform& t, int32 hexColor) {
  float32 vertices[] = {
    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,

    // Right face
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

    // Left face
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  };

  uint32 indices[] = {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
  };

  EntityAttributes attr;
  attr.vertices = vertices;
  attr.vertArrSize = sizeof(vertices);
  attr.vertexCount = 24;
  attr.vertexSize = 3;
  attr.transform = t;

  attr.indices = indices;
  attr.indArrSize = sizeof(indices);

  attr.hexColor = hexColor;
  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}
