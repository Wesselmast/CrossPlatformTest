#define __WINDOWS__

#include <cstdio>

static bool globalConfinedCursor;
void confine_cursor(bool confine);

#include "Application.cpp"

#include <chrono>
#include <ctime>

#include <windows.h>

static const char* windowName = "Political Terrain";
typedef BOOL fptr_wglSwapIntervalEXT(int interval);

static Input* input;
static OpenGLState* state;
static AppState* app;

static HDC   hdc; 
static HGLRC hrc; 

static int32 globalMouseX;
static int32 globalMouseY;

void confine_cursor(bool confine) {
  if(confine) {
    RECT rect;
    HWND activeWindow = GetForegroundWindow();
    GetClientRect(activeWindow, &rect);
    MapWindowPoints(activeWindow, nullptr, (POINT*)(&rect), 2);
    ClipCursor(&rect); 
  }
  else {
    ClipCursor(nullptr);
  }
  ShowCursor(!confine);
  printf("confined: %d", confine);
  globalConfinedCursor = confine;
} 

void setup_pixel_format(HDC hdc) {
  PIXELFORMATDESCRIPTOR  pfd;
  PIXELFORMATDESCRIPTOR* ppfd = &pfd;

  ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
  ppfd->nVersion = 1;
  ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  ppfd->dwLayerMask = PFD_MAIN_PLANE;
  ppfd->iPixelType = PFD_TYPE_RGBA;
  ppfd->cColorBits = 32;
  ppfd->cDepthBits = 24;

  int32 pixelFormat = ChoosePixelFormat(hdc, ppfd);
  HRESULT pixelFormatResult = SetPixelFormat(hdc, pixelFormat, &pfd); 
}

#define PROC_DEFAULT DefWindowProc(hwnd, uMsg, wParam, lParam);

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;

  switch(uMsg) {
    case WM_CREATE: {
      hdc = GetDC(hwnd); 
      setup_pixel_format(hdc);
      hrc = wglCreateContext(hdc);
      wglMakeCurrent(hdc, hrc);

      state = gl_start();
      input = input_start();
      app = app_start(state, input);
      return 0;
    }
    case WM_PAINT: {
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_ACTIVATE: {
      SetForegroundWindow(hwnd);
      return 0;
    }
    case WM_SIZE: {
      if(state) {
	gl_resize(state, LOWORD(lParam), HIWORD(lParam));
	return 0;
      }
      return 1;
    }
    case WM_MOUSEMOVE: {
      globalMouseX = LOWORD(lParam);
      globalMouseY = HIWORD(lParam);
      return 0;
    }
    case WM_LBUTTONDOWN: {
      SetForegroundWindow(hwnd);
      return 0;
    }
    case WM_LBUTTONUP: {
      // add key state stuff later
      return 0;
    }

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN: {
      set_key_state(input, wParam, 1);
      return 0;
    }
    
    case WM_SYSKEYUP:
    case WM_KEYUP: {
      set_key_state(input, wParam, 0);
      return 0;
    }

    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
    case WM_CLOSE: {
      DestroyWindow(hwnd);
      return 0;
    }
    default: return PROC_DEFAULT;
  }
}

HWND create_window(uint16 width, uint16 height, const char* name) {
  WNDCLASS windowClass = { };
  
  windowClass.style = 0;
  windowClass.lpfnWndProc = window_proc;
  windowClass.hInstance = GetModuleHandle(0);
  windowClass.lpszClassName = "CustomFloatingWindow";
  windowClass.hIcon = LoadIcon(0, IDI_INFORMATION);
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  
  HRESULT registerResult = RegisterClass(&windowClass); 
  
  HWND hwnd = CreateWindowEx(0, windowClass.lpszClassName, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			     CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, windowClass.hInstance, 0);

  return hwnd;
}

void close_window(HWND& hwnd, HDC& hdc, HGLRC& hrc) {
  wglMakeCurrent(0, 0);
  wglDeleteContext(hrc);
  ReleaseDC(hwnd, hdc);
} 

#if 0
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
#else
int main() {
#endif
  HWND window = create_window(1280, 720, windowName);
  if(!window) {
    log_("window couldn't be created!\n");
    return 0;
  }

  ShowWindow(window, SW_SHOW);
  UpdateWindow(window);

  confine_cursor(true);

  std::chrono::high_resolution_clock timer;
  float64 time = 0.0;
  float64 dt = 0.0;

  MSG message;
  while(1) {
    auto start = timer.now();

    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
      if(message.message == WM_QUIT) goto quit;
      TranslateMessage(&message);
      DispatchMessage(&message);
    }

    int32& x = globalMouseX;
    int32& y = globalMouseY;
    set_mouse_moved(input, x, y);

    if(globalConfinedCursor) {
      uint16& ww = state->windowWidth;
      uint16& wh = state->windowHeight;
      const int32 bfr = 10;
      if(x < bfr || x > ww - bfr || y < bfr || y > wh - bfr) {
        uint16 midX = ww / 2;
        uint16 midY = wh / 2;
        SetCursorPos(midX, midY);
        reset_mouse(input, midX, midY);
      }
    }
    
    input_tick(input); 
    if(app_tick(state, input, app, dt, time)) PostQuitMessage(0);
    gl_tick(state, app->camera);

    SwapBuffers(hdc);

    dt = std::chrono::duration<float64>(timer.now() - start).count();
    time += dt;
  }
 quit:
  gl_end(state);
  input_end(input);
  app_end(app);

  close_window(window, hdc, hrc);
  return 0;
}
