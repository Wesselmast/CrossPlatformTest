#pragma once
#include "Types.cpp"
#include "Math.cpp"
#include "Color.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Uniforms.cpp"
#include "Batch.cpp"

struct OpenGLState {
  BatchMap batches;
  UniformList globalUniforms;

  int32 amtOfLights;
  
  Mat4 vp;
  Vec3 cameraPos;
  uint16 windowWidth;
  uint16 windowHeight;
};

#include "Actor.cpp"
#include "RendererComponent.cpp"
#include "CameraComponent.cpp"

#define RENDERMODE_NORMAL     GL_FILL
#define RENDERMODE_WIREFRAME  GL_LINE
#define RENDERMODE_POINT      GL_POINT

static int32 globalRenderUnlit;

inline void set_rendermode(int32 renderMode) {
  glPolygonMode(GL_FRONT_AND_BACK, renderMode);
  globalRenderUnlit = renderMode != RENDERMODE_NORMAL;
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

  OpenGLState* state = new OpenGLState;

  uniform(state->globalUniforms, "camPos",      &state->cameraPos);
  uniform(state->globalUniforms, "viewProj",    &state->vp);
  uniform(state->globalUniforms, "amtOfLights", &state->amtOfLights);

  uniform(state->globalUniforms, "renderUnlit", &globalRenderUnlit);

  return state;
}

void gl_tick(OpenGLState* state, Actor* c) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  get_component<CameraComponent*>(c, "camera")->update(state);

  for(auto& matB : state->batches.map) {
    matB.first->render(state->globalUniforms);
    for(auto& meshB : matB.second) {
      meshB.first->prepare_draw();
      for(TransformComponent* t : meshB.second) {
	t->render(matB.first->program);
	meshB.first->draw();
      }
    }
  }
}

void gl_end(OpenGLState* state) {
  //later add in checking for level saving

  uniform_free_list(state->globalUniforms);
  free(state);
}

void gl_resize(OpenGLState* state, uint16 w, uint16 h) {
  state->windowWidth = w;
  state->windowHeight = h;
  glViewport(0, 0, w, h);
} 
