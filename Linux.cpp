#include <cstdio>
#include "OpenGL.cpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

void expose(const XEvent& e, Display* d, Window& w) {
  XWindowAttributes wa;
  XGetWindowAttributes(d, w, &wa);
  glViewport(0, 0, wa.width, wa.height);
  draw_quad();
  glXSwapBuffers(d, w); 
}

bool key(const XEvent& e, bool down) {
  printf("%s key: %x\n", down ? "Pressed" : "Released", e.xkey.keycode);
  return e.xkey.keycode == 9;
}

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

  bool quit = false;
  XEvent e;

  while(!quit) {
    XNextEvent(d, &e);
    switch(e.type) {
      case Expose:   	expose(e, d, w);       break;
      case KeyPress:    quit |= key(e, true);  break;
      case KeyRelease:  quit |= key(e, false); break;
    }
  }

  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
