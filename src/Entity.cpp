#pragma once

#include "Types.cpp"
#include "Math.cpp"
#include "File.cpp"
#include "Color.cpp"
#include "MeshGeneration.cpp"
#include "Layout.cpp"

#include <vector>
#include <string>

struct EntityAttributes {
  float32* vertices;
  uint32 vertexCount; 
  uint32 vertArrSize;
  uint8 vertexLayout;
  
  uint32* indices;
  uint32 indArrSize;
  
  std::string vertexShaderPath;
  std::string fragmentShaderPath;

  Transform transform;
};

struct PBR {
  int32 hexColor;
  float32 metallic;
  float32 roughness;
};

struct Entity {
  std::forward_list<BaseUniform*> uniforms;

  uint32 program;
  
  uint32 vertexBuffer;
  uint32 vertexCount;
  uint8 vertexLayout;

  uint32 indexBuffer;
  uint32 indexBufferSize;  

  uint32 textureID;
  int32 textureType = GL_TEXTURE_2D;

  int32 cullFace = GL_FRONT;
  int32 depthFunc = GL_LESS;

  Mat4 modelMatrix;
  Mat4 normalMatrix;
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

  void draw(OpenGLState* state) {
    glDepthFunc(depthFunc);
    glCullFace(cullFace);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    activate_layout(vertexLayout);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(textureType, textureID);

    glUseProgram(program);

    uniform_tick_list(state->globalUniforms, program); 
    uniform_tick_list(uniforms, program);

    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, (void*)0);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LESS);
    glUseProgram(0);
    glDisableVertexAttribArray(0);
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
    normalMatrix = mat4_transpose(mat4_inverse(modelMatrix)); 
    this->transform = t;
  }

  Entity* reinit(EntityAttributes* attr) {
    vertexCount = attr->vertexCount;
    vertexLayout = attr->vertexLayout;
    indexBufferSize = attr->indArrSize / sizeof(uint32);

    load_shaders(attr->vertexShaderPath.c_str(), attr->fragmentShaderPath.c_str());

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, attr->vertArrSize, attr->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, attr->indArrSize, attr->indices, GL_STATIC_DRAW);

    transform = attr->transform;
    set_transform(transform);

    return this;
  }

  Entity(OpenGLState* state, EntityAttributes* attr) {
    state->entities.emplace_front(reinit(attr));
  }
};

void destroy_entity_partial(Entity* entity) {
  if(!entity) return;
  uniform_free_list(entity->uniforms);
}

void destroy_entity(OpenGLState* state, Entity* entity) {
  if(!entity) return;
  destroy_entity_partial(entity);
  state->entities.remove(entity);
}

Entity* rect(OpenGLState* state, const Transform& t, const PBR& pbr) {
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
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/defaultV.glsl";
  attr.fragmentShaderPath = "res/shaders/defaultF.glsl";

  attr.indices = indices;
  attr.indArrSize = sizeof(indices);

  Entity* e = new Entity(state, &attr);

  Color color = hex_to_color(pbr.hexColor);
  float32 metallic = pbr.metallic;
  float32 roughness = pbr.roughness;

  uniform(e->uniforms, "albedo",    color);
  uniform(e->uniforms, "metallic",  metallic);
  uniform(e->uniforms, "roughness", roughness);
  uniform(e->uniforms, "model",     &e->modelMatrix);
  uniform(e->uniforms, "normalMat", &e->normalMatrix);

  return e;
}

Entity* cube(OpenGLState* state, const Transform& t, const PBR& pbr) {
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
    -1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,

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
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/vertex_PBR.glsl";
  attr.fragmentShaderPath = "res/shaders/fragment_PBR.glsl";

  attr.indices = indices;
  attr.indArrSize = sizeof(indices);

  Entity* e = new Entity(state, &attr);
  
  Color color = hex_to_color(pbr.hexColor);
  float32 metallic = pbr.metallic;
  float32 roughness = pbr.roughness;

  uniform(e->uniforms, "albedo",    color);
  uniform(e->uniforms, "metallic",  metallic);
  uniform(e->uniforms, "roughness", roughness);
  uniform(e->uniforms, "model",     &e->modelMatrix);
  uniform(e->uniforms, "normalMat", &e->normalMatrix);

  return e; 
}

Entity* sphere(OpenGLState* state, const Transform& t, const PBR& pbr, int32 res = 50) {
  int32 resSqr = res * res;

  std::vector<float32> vertices(resSqr * 6, 0.0f);
  std::vector<uint32>  indices;

  int32 i = 0;
  for(int32 z = 0; z < res; ++z) {
    float32 angle1 = z * pi() / (res - 1);
    for(int32 x = 0; x < res; ++x) {
      float32 angle2 = x * (pi() * 2) / (res - 1);

      vertices[(i * 6) + 0] = sin(angle1) * cos(angle2);
      vertices[(i * 6) + 1] = cos(angle1);
      vertices[(i * 6) + 2] = sin(angle1) * sin(angle2);

      generate_index(indices, i, z, res, true);
      ++i;
    }
  }

  generate_normals(vertices, indices);

  EntityAttributes attr;
  attr.vertices = &vertices[0];
  attr.vertArrSize = resSqr * 6 * sizeof(float32);
  attr.vertexCount = resSqr;
  attr.vertexLayout = LAYOUT_POSITION_3D | LAYOUT_NORMAL; 
  attr.transform = t;
  attr.vertexShaderPath   = "res/shaders/vertex_PBR.glsl";
  attr.fragmentShaderPath = "res/shaders/fragment_PBR.glsl";

  attr.indices = &indices[0];
  attr.indArrSize = indices.size() * sizeof(uint32);

  Entity* e = new Entity(state, &attr);
  
  Color color = hex_to_color(pbr.hexColor);
  float32 metallic = pbr.metallic;
  float32 roughness = pbr.roughness;

  uniform(e->uniforms, "albedo",    color);
  uniform(e->uniforms, "metallic",  metallic);
  uniform(e->uniforms, "roughness", roughness);
  uniform(e->uniforms, "model",     &e->modelMatrix);
  uniform(e->uniforms, "normalMat", &e->normalMatrix);

  return e; 
}
