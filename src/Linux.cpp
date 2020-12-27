#define __LINUX__

#include <cstdio>
#include <chrono>
#include "Application.cpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

void expose(OpenGLState* state, Display* d, Window& w) {
  XWindowAttributes wa;
  XGetWindowAttributes(d, w, &wa);
  
  state->windowWidth = wa.width;
  state->windowHeight = wa.height;
}

void get_mouse_position(Display* d, Window& w, int32* x, int32* y) {
  Window tempW;
  int32 tempX, tempY;
  uint32 tempM;

  XQueryPointer(d, w, &tempW, &tempW, x, y, &tempX, &tempY, &tempM);
}

int main() {
  Display* d = XOpenDisplay(0);
  Window root = DefaultRootWindow(d);

  GLint glAtt[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo* vi = glXChooseVisual(d, 0, glAtt);

  Colormap cmap = XCreateColormap(d, root, vi->visual, AllocNone);
  XSetWindowAttributes xAtt;
  xAtt.colormap = cmap;
  xAtt.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask;

  Window w = XCreateWindow(d, root, 0, 0, 300, 300, 0, vi->depth, InputOutput, 
			   vi->visual, CWColormap | CWEventMask, &xAtt);
  XMapWindow(d, w);
  XStoreName(d, w, "First GNU/Linux App");

  GLXContext glc = glXCreateContext(d, vi, 0, GL_TRUE);
  glXMakeCurrent(d, w, glc);

  OpenGLState* state = gl_start();
  Input* input = input_start();

  app_start(state, input);

  std::chrono::high_resolution_clock timer;
  float64 time = 0;
  float64 dt = 0;
  
  XEvent e;
  while(1) {
    auto start = timer.now();

    if(XPending(d) > 0) {
      XNextEvent(d, &e);
      switch(e.type) {
	case Expose:        expose(state, d, w); break;
	case KeyPress:      set_key_state(input, e.xkey.keycode,   1); break; 
	case KeyRelease:    set_key_state(input, e.xkey.keycode,   0); break; 
	case ButtonPress:   set_key_state(input, e.xbutton.button, 1); break; 
	case ButtonRelease: set_key_state(input, e.xbutton.button, 0); break; 
      }
    }

    int32 x, y;
    get_mouse_position(d, w, &x, &y);
    set_mouse_moved(input, x, y); 

    input_tick(input, state);  		  //input
    if(app_tick(state, input, dt)) break; //simulate
    gl_tick(state);            	          //render
    glXSwapBuffers(d, w);

    dt = std::chrono::duration<double>(timer.now() - start).count();
    time += dt;
  }

  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
