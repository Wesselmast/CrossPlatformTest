#pragma once
#include "Types.cpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

void draw_quad() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 20.0f);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  glBegin(GL_QUADS);
  glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.75f, -0.75f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( 0.75f, -0.75f, 0.0f);
  glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 0.75f,  0.75f, 0.0f);
  glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(-0.75f,  0.75f, 0.0f);
  glEnd();
}

void gl_start() {
  if(glewInit() != GLEW_OK) {
    printf("glew is bogged!");
  }

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
