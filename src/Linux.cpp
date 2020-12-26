#include <cstdio>
#include "OpenGL.cpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

void expose(OpenGLState* state, Display* d, Window& w) {
  XWindowAttributes wa;
  XGetWindowAttributes(d, w, &wa);
  
  state->windowWidth = wa.width;
  state->windowHeight = wa.height;
}

bool key(OpenGLState* state, const XEvent& e, bool down) {
  Camera* c = state->camera;
  float x = c->position.x;
  float y = c->position.y;
  float z = c->position.z;

  float xr = c->rotation.x;
  float yr = c->rotation.y;
  float zr = c->rotation.z;
  const float s = 1;
  const float sr = 22.5f * d2r();

  switch(e.xkey.keycode) {
    case 25: z += s;   break; //W
    case 39: z -= s;   break; //S
    case 38: x -= s;   break; //A
    case 40: x += s;   break; //D
    case 24: zr -= sr; break; //Q
    case 26: xr += sr; break; //E
    case  9: return 1;
    default: log_("%s key: %d\n", down ? "Pressed" : "Released", e.xkey.keycode);
  }

  c->set_transform(state, {x,y,z}, {xr,yr,zr});
  return 0;
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

  OpenGLState* state = gl_start();

  rect(state, {0.0f, 0.0f}, 1, {104.0f/255.0f, 182.0f/255.0f, 132.0f/255.0f});

  bool quit = false;
  XEvent e;

  while(!quit) {
    if(XPending(d) > 0) {
      XNextEvent(d, &e);
    
      switch(e.type) {
	case Expose:   	 expose(state, d, w);          break;
	case KeyPress:   quit |= key(state, e, true);  break;
	case KeyRelease: quit |= key(state, e, false); break;
      }
    }

    gl_render(state);
    glXSwapBuffers(d, w); 
  }

  gl_end(state);
  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
