#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "File.cpp"
#include "List.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* these should later be removed */
#include "glm/glm.hpp" 
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Entity;

struct OpenGLState {
  List<Entity*> entities;
  uint16 windowWidth;
  uint16 windowHeight;
};

struct EntityAttributes {
  float32* vertices;
  uint32 size;
  Vec3 color;
  uint32 vertexCount;
};

struct Entity {
  uint32 program;
  uint32 vertexBuffer;
  uint32 vertexCount;
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
    
    load_shaders("res/shaders/defaultV.glsl", "res/shaders/defaultF.glsl");
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, attr->size, attr->vertices, GL_STATIC_DRAW);

    state->entities.insert(this);
    return this;
  }
};

Entity* rect(OpenGLState* state, const Vec2& position, float32 scale, const Vec3& color) {
  const float32& x = position.x;
  const float32& y = position.y;

  float32 vertices[] = {
    x - scale, y - scale,
    x + scale, y - scale,
    x + scale, y + scale,
    x - scale, y - scale,
    x - scale, y + scale,
    x + scale, y + scale,
  };

  EntityAttributes attr;
  attr.vertices = vertices;
  attr.size = sizeof(vertices);
  attr.vertexCount = 6;
  attr.color = color;

  return ((Entity*)malloc(sizeof(Entity)))->init(state, &attr);
}

OpenGLState* gl_start() {
  if(glewInit() != GLEW_OK) {
    printf("glew is bogged!");
    return nullptr;
  }

  OpenGLState* state = (OpenGLState*)malloc(sizeof(OpenGLState));

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
 // Mat4 proj = mat4_perspective_fov(68.0f, (float32)w/(float32)h, 0.1f, 1000.0f); 
  glm::mat4 proj = glm::ortho(0.0f, (float32)w, 0.0f, (float32)h);  

  Node<Entity*>* current = state->entities.head;
  while(current) {
    Entity* e = current->data;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, e->vertexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glUseProgram(e->program);

    float32& r = e->color.x;
    float32& g = e->color.y;
    float32& b = e->color.z;

    glUniform3f(glGetUniformLocation(e->program, "objColor"), r, g, b);
    glUniformMatrix4fv(glGetUniformLocation(e->program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
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
}

