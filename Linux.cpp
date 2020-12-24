#include <cstdio>
#include "OpenGL.cpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

int main() {
  GLint glAtt[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE,
    24,
    GLX_DOUBLEBUFFER,
    None
  };

  Display* d = XOpenDisplay(0);

  Window root = DefaultRootWindow(d);
  XVisualInfo* vi = glXChooseVisual(d, 0, glAtt);

  Colormap cmap = XCreateColormap(d, root, vi->visual, AllocNone);
  XSetWindowAttributes xAtt;
  xAtt.colormap = cmap;
  xAtt.event_mask = ExposureMask | KeyPressMask;

  Window w = XCreateWindow(d, root, 0, 0, 300, 300, 0, vi->depth, InputOutput, 
			   vi->visual, CWColormap | CWEventMask, &xAtt);
  XMapWindow(d, w);
  XStoreName(d, w, "First GNU/Linux App");

  GLXContext glc = glXCreateContext(d, vi, 0, GL_TRUE);
  glXMakeCurrent(d, w, glc);

  gl_start();

  XWindowAttributes wa;
  XEvent e;
  do {
    XNextEvent(d, &e);

    if(e.type == Expose) {
      XGetWindowAttributes(d, w, &wa);
      glViewport(0, 0, wa.width, wa.height);
      draw_quad();
      glXSwapBuffers(d, w);
    }
  }
  while(e.type != ButtonPress);

  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
