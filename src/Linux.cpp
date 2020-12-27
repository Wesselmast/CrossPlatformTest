#define __LINUX__

#include <cstdio>
#include <chrono>
#include "OpenGL.cpp"
#include "Input.cpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

void expose(OpenGLState* state, Display* d, Window& w) {
  XWindowAttributes wa;
  XGetWindowAttributes(d, w, &wa);
  
  state->windowWidth = wa.width;
  state->windowHeight = wa.height;
}

bool simulate_app(OpenGLState* state, Input* input, float64 dt) {
  Camera* c = state->camera;
  float32 x = c->position.x;
  float32 y = c->position.y;
  float32 z = c->position.z;

  float32 xr = c->rotation.x;
  float32 yr = c->rotation.y;
  float32 zr = c->rotation.z;
  const float32 s = 10 * dt;
  const float32 sr = 45 * d2r() * dt;

  z  += is_down(input, KEY_W) * s;
  z  -= is_down(input, KEY_S) * s;
  x  += is_down(input, KEY_D) * s;
  x  -= is_down(input, KEY_A) * s;
  zr += is_down(input, KEY_E) * sr;
  zr -= is_down(input, KEY_Q) * sr;

  c->set_transform(state, {x,y,z}, {xr,yr,zr});
  return is_down(input, KEY_ESCAPE);
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
  xAtt.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask;

  Window w = XCreateWindow(d, root, 0, 0, 300, 300, 0, vi->depth, InputOutput, 
			   vi->visual, CWColormap | CWEventMask, &xAtt);
  XMapWindow(d, w);
  XStoreName(d, w, "First GNU/Linux App");

  GLXContext glc = glXCreateContext(d, vi, 0, GL_TRUE);
  glXMakeCurrent(d, w, glc);

  OpenGLState* state = gl_start();
  Input* input = input_start();

  rect(state, { 0.0f,  0.0f,   0.0f}, 1, {104.0f/255.0f, 182.0f/255.0f, 132.0f/255.0f});
  rect(state, { 1.0f,  4.0f,  -1.0f}, 1, { 53.0f/255.0f, 203.0f/255.0f,  93.0f/255.0f});
  rect(state, {-4.0f, -2.0f,   3.0f}, 1, {230.0f/255.0f,  44.0f/255.0f,  20.0f/255.0f});

  std::chrono::high_resolution_clock timer;
  float64 time = 0;
  float64 dt = 0;
  
  bool quit = false;
  XEvent e;

  while(!quit) {
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

    input_tick(input, state);  		      //input
    quit = simulate_app(state, input, dt);    //simulate
    gl_render(state);            	      //render
    glXSwapBuffers(d, w);

    dt = std::chrono::duration<double>(timer.now() - start).count();
    time += dt;
  }

  input_end(input);
  gl_end(state);
  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
