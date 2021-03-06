#define __LINUX__

#include <cstdio>

static bool globalConfinedCursor;
void confine_cursor(bool confine);

#include "Application.cpp"

#include <chrono>
#include <ctime>

#include <X11/Xlib.h>
#include <GL/glx.h>

static Display* d;
static Window w;
static Cursor emptyCursor;

inline void expose(OpenGLState* state) {
  XWindowAttributes wa;
  XGetWindowAttributes(d, w, &wa);
  
  gl_resize(state, wa.width, wa.height);
}

inline void get_mouse_position(int32* x, int32* y) {
  Window tempW;
  int32 tempX, tempY;
  uint32 tempM;

  XQueryPointer(d, w, &tempW, &tempW, &tempX, &tempY, x, y, &tempM);
}


void allocate_empty_cursor() {
  XColor col = {0};
  const char data[] = {0};

  Pixmap pixmap = XCreateBitmapFromData(d, w, data, 1, 1);
  emptyCursor   = XCreatePixmapCursor(d, pixmap, pixmap, &col, &col, 0, 0);

  XFreePixmap(d, pixmap);
}

void confine_cursor(bool confine) {
  if(confine) {
    XGrabPointer(d, w, true, 0, GrabModeAsync, GrabModeAsync, w, emptyCursor, CurrentTime);
  }
  else {
    XGrabPointer(d, w, true, 0, GrabModeAsync, GrabModeAsync, w, None, CurrentTime);
  }
  globalConfinedCursor = confine;
}

int main() {
  srand(time(0));

  d = XOpenDisplay(0);
  Window root = DefaultRootWindow(d);

  GLint glAtt[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo* vi = glXChooseVisual(d, 0, glAtt);

  Colormap cmap = XCreateColormap(d, root, vi->visual, AllocNone);
  XSetWindowAttributes xAtt;
  xAtt.colormap = cmap;
  xAtt.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask;

  w = XCreateWindow(d, root, 0, 0, 500, 500, 0, vi->depth, InputOutput, 
		    vi->visual, CWColormap | CWEventMask, &xAtt);
  XMapWindow(d, w);
  XStoreName(d, w, "First GNU/Linux App");

  GLXContext glc = glXCreateContext(d, vi, 0, GL_TRUE);
  glXMakeCurrent(d, w, glc);

  allocate_empty_cursor();
  confine_cursor(true);

  OpenGLState* state = gl_start();
  Input* input = input_start();
  AppState* app = app_start(state, input);

  std::chrono::high_resolution_clock timer;
  float64 time = 0;
  float64 dt = 0;
  
  XEvent e;
  while(1) {
    auto start = timer.now();

    while(XPending(d) > 0) {
      XNextEvent(d, &e);
      switch(e.type) {
	case Expose:        expose(state); break;
	case KeyPress:      set_key_state(input, e.xkey.keycode,   1); break; 
	case KeyRelease:    set_key_state(input, e.xkey.keycode,   0); break; 
	case ButtonPress:   set_key_state(input, e.xbutton.button, 1); break; 
	case ButtonRelease: set_key_state(input, e.xbutton.button, 0); break; 
      }
    }

    int32 x, y;
    get_mouse_position(&x, &y);
    set_mouse_moved(input, x, y);

    if(globalConfinedCursor) {
      uint16& ww = state->windowWidth;
      uint16& wh = state->windowHeight;
      const int32 bfr = 10;
      if(x < bfr || x > ww - bfr || y < bfr || y > wh - bfr) {
        uint16 midX = ww / 2;
        uint16 midY = wh / 2;
        XWarpPointer(d, None, w, 0, 0, 0, 0, midX, midY);
        reset_mouse(input, midX, midY);
      }
    }

    input_tick(input); 
    if(app_tick(state, input, app, dt, time)) break;
    gl_tick(state, app->camera);
    glXSwapBuffers(d, w);

    dt = std::chrono::duration<float64>(timer.now() - start).count();
    time += dt;
  }

  gl_end(state);
  input_end(input);
  app_end(app);

  glXMakeCurrent(d, None, 0);
  glXDestroyContext(d, glc);
  XFreeCursor(d, emptyCursor);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
  return 0;
}
