#define __WINDOWS__

#include <cstdio>
#include <chrono>
#include "Application.cpp"

#include <windows.h>

static const char* windowName = "Political Terrain";
typedef BOOL fptr_wglSwapIntervalEXT(int interval);

static Input* input;
static OpenGLState* state;
static AppState* app;

void lock_mouse(bool confine) {
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
} 

#define PROC_DEFAULT DefWindowProc(hwnd, uMsg, wParam, lParam);

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_ACTIVATE: {
    SetForegroundWindow(hwnd);
    lock_mouse(wParam & WA_ACTIVE);
    return PROC_DEFAULT;
  }
  case WM_SIZE: {
    if(state) gl_resize(state, LOWORD(lParam), HIWORD(lParam));
    return PROC_DEFAULT;
  }
  case WM_MOUSEMOVE: {
    set_mouse_moved(input, LOWORD(lParam), HIWORD(lParam));
    return PROC_DEFAULT;
  }
  case WM_LBUTTONDOWN: {
    SetForegroundWindow(hwnd);
    return PROC_DEFAULT;
  }
  case WM_LBUTTONUP: {
    // add key state stuff later
    return PROC_DEFAULT;
  }

  case WM_SYSKEYDOWN:
  case WM_KEYDOWN: {
    set_key_state(input, wParam, 1);
    return PROC_DEFAULT;
  }
  
  case WM_KEYUP: 
  case WM_SYSKEYUP: {
    set_key_state(input, wParam, 0);
    return PROC_DEFAULT;
  }

//  case WM_INPUT: {
//    uint32 size;
//    if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1) {
//      log_("input missed 1\n");
//      return 0;
//    }
//    char raw[size];
//    if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (void*)(&raw), &size, sizeof(RAWINPUTHEADER)) != size) { 
//      log_("input missed 2\n");
//      return 0;
//    }
//
//    const RAWINPUT& rawInput = (const RAWINPUT&)(raw);
//    
//    bool32 rawInputFlags = rawInput.header.dwType == RIM_TYPEMOUSE && (rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0);
//    if(rawInputFlags) {
//      input.rawMousePosition.x += rawInput.data.mouse.lLastX;
//      input.rawMousePosition.y += rawInput.data.mouse.lLastY;
//    }
//    return 0;
//  }

  case WM_DESTROY: {
    PostQuitMessage(0);
    return PROC_DEFAULT;
  }
  case WM_CLOSE: {
    DestroyWindow(hwnd);
    return PROC_DEFAULT;
  }
  }
  return PROC_DEFAULT;
}

HWND create_window(uint16 width, uint16 height, const char* name, HDC& hdc, HGLRC& hrc) {
  WNDCLASS windowClass = { };
  
  windowClass.style = CS_OWNDC;
  windowClass.lpfnWndProc = window_proc;
  windowClass.hInstance = GetModuleHandle(0);
  windowClass.lpszClassName = "CustomFloatingWindow";
  windowClass.hIcon = LoadIcon(0, IDI_INFORMATION);
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  
  HRESULT registerResult = RegisterClass(&windowClass); 
  
  HWND hwnd = CreateWindowEx(0, windowClass.lpszClassName, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			     CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, windowClass.hInstance, 0);

  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;

  hdc = GetDC(hwnd);
  int32 pixelFormat = ChoosePixelFormat(hdc, &pfd);
  HRESULT pixelFormatResult = SetPixelFormat(hdc, pixelFormat, &pfd); 
  
  DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  hrc = wglCreateContext(hdc);
  wglMakeCurrent(hdc, hrc);

  ShowWindow(hwnd, SW_SHOW);
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
  HDC hdc;
  HGLRC hrc;
  HWND window = create_window(500, 500, windowName, hdc, hrc);

  fptr_wglSwapIntervalEXT* wglSwapInterval = (fptr_wglSwapIntervalEXT*)wglGetProcAddress("wglSwapIntervalEXT");
  if(wglSwapInterval) {
    wglSwapInterval(1);
  }

  RAWINPUTDEVICE rawInputDevice = {};
  rawInputDevice.usUsagePage = 0x01;
  rawInputDevice.usUsage = 0x02;
  rawInputDevice.dwFlags = 0;
  rawInputDevice.hwndTarget = nullptr;
  RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE));
     
  state = gl_start();
  input = input_start();
  app = app_start(state, input);
 
  std::chrono::high_resolution_clock timer;
  float64 time = 0.0;
  float64 dt = 0.0;
  
  MSG message;
  while(true) {
    auto start = timer.now();
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
      if(message.message == WM_QUIT) goto quit;
      TranslateMessage(&message);
      DispatchMessage(&message);
    }

  //  input_tick(input); 
  //  if(app_tick(state, input, app, dt, time)) PostQuitMessage(0);
  //  gl_tick(state, app->camera);
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
