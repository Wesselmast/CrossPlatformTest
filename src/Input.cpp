#pragma once
#include <forward_list>

#if defined(__LINUX__)
enum {
  //Mouse Events
  MOUSE_L           = 1,
  MOUSE_M           = 2,
  MOUSE_R           = 3,
  MOUSE_SCROLL_UP   = 4,
  MOUSE_SCROLL_DOWN = 5,

  //Key Events
  KEY_ESCAPE        = 9,
  KEY_BACKSPACE     = 22,
  KEY_TAB 	    = 23,
  KEY_ENTER 	    = 36,
  KEY_CTRL 	    = 37,
  KEY_SHIFT 	    = 50,
  KEY_ALT 	    = 64,
  KEY_SPACE 	    = 65,
  KEY_DELETE 	    = 107,
  
  KEY_1 	    = 10,
  KEY_2 	    = 11,
  KEY_3 	    = 12,
  KEY_4 	    = 13,
  KEY_5 	    = 14,
  KEY_6 	    = 15,
  KEY_7 	    = 16,
  KEY_8 	    = 17,
  KEY_9 	    = 18,
  KEY_0 	    = 19,
  
  KEY_Q 	    = 24,
  KEY_W 	    = 25,
  KEY_E 	    = 26,
  KEY_R 	    = 27,
  KEY_T 	    = 28,
  KEY_Y 	    = 29,
  KEY_U 	    = 30,
  KEY_I 	    = 31,
  KEY_O 	    = 32,
  KEY_P 	    = 33,

  KEY_A 	    = 38,
  KEY_S 	    = 39,
  KEY_D 	    = 40,
  KEY_F 	    = 41,
  KEY_G 	    = 42,
  KEY_H 	    = 43,
  KEY_J 	    = 44,
  KEY_K 	    = 45,
  KEY_L 	    = 46,

  KEY_Z 	    = 52,
  KEY_X		    = 53,
  KEY_C 	    = 54,
  KEY_V		    = 55,
  KEY_B 	    = 56,
  KEY_N 	    = 57,
  KEY_M 	    = 58,

  KEY_F1 	    = 67,
  KEY_F2 	    = 68,
  KEY_F3 	    = 69,
  KEY_F4 	    = 70,
  KEY_F5 	    = 71,
  KEY_F6 	    = 72,
  KEY_F7 	    = 73,
  KEY_F8 	    = 74,
  KEY_F9 	    = 75,
  KEY_F10	    = 76,
  KEY_F11	    = 95,
  KEY_F12	    = 96,

  MAXKEYVALUE       = KEY_DELETE + 1
};  

#elif defined(__WINDOWS__)
enum {
  //Mouse Events
  MOUSE_L           = 0x01,
  MOUSE_M           = 0x04,
  MOUSE_R           = 0x02,
//  MOUSE_SCROLL_UP   = 4,          Windows doesn't have these. should be fixed
//  MOUSE_SCROLL_DOWN = 5,

  //Key Events
  KEY_ESCAPE        = 0x1B,
  KEY_BACKSPACE     = 0x08,
  KEY_TAB 	    = 0x09,
  KEY_ENTER 	    = 0x0D,
  KEY_CTRL 	    = 0x11,
  KEY_SHIFT 	    = 0x10,
  KEY_ALT 	    = 0x12,
  KEY_SPACE 	    = 0x20,
  KEY_DELETE 	    = 0x2E,
  
  KEY_1 	    = 0x31,
  KEY_2 	    = 0x32,
  KEY_3 	    = 0x33,
  KEY_4 	    = 0x34,
  KEY_5 	    = 0x35,
  KEY_6 	    = 0x36,
  KEY_7 	    = 0x37,
  KEY_8 	    = 0x38,
  KEY_9 	    = 0x39,
  KEY_0 	    = 0x30,
  
  KEY_A		    = 0x41,
  KEY_B		    = 0x42,
  KEY_C		    = 0x43,
  KEY_D		    = 0x44,
  KEY_E		    = 0x45,
  KEY_F		    = 0x46,
  KEY_G		    = 0x47,
  KEY_H		    = 0x48,
  KEY_I		    = 0x49,
  KEY_J		    = 0x4A,
  KEY_K		    = 0x4B,
  KEY_L		    = 0x4C,
  KEY_M		    = 0x4D,
  KEY_N		    = 0x4E,
  KEY_O		    = 0x4F,
  KEY_P		    = 0x50,
  KEY_Q		    = 0x51,
  KEY_R		    = 0x52,
  KEY_S		    = 0x53,
  KEY_T		    = 0x54,
  KEY_U		    = 0x55,
  KEY_V		    = 0x56,
  KEY_W		    = 0x57,
  KEY_X		    = 0x58,
  KEY_Y		    = 0x59,
  KEY_Z		    = 0x5A,

  KEY_F1 	    = 0x70,
  KEY_F2 	    = 0x71,
  KEY_F3 	    = 0x72,
  KEY_F4 	    = 0x73,
  KEY_F5 	    = 0x74,
  KEY_F6 	    = 0x75,
  KEY_F7 	    = 0x76,
  KEY_F8 	    = 0x77,
  KEY_F9 	    = 0x78,
  KEY_F10	    = 0x79,
  KEY_F11	    = 0x7A,
  KEY_F12	    = 0x7B,

  MAXKEYVALUE       = KEY_F12 + 1
};
#endif

typedef void(*fptr_keyevent)();
typedef std::forward_list<fptr_keyevent> KeyList;

struct Input {
  bool keyInput[MAXKEYVALUE];
  bool keyFlags[MAXKEYVALUE];
  KeyList keyDownEvents[MAXKEYVALUE];
  KeyList keyUpEvents[MAXKEYVALUE];
  int32 mouseX = 0;
  int32 mouseY = 0;
  int32 lastMouseX = 0;
  int32 lastMouseY = 0;
  int32 mouseXDiff = 0;
  int32 mouseYDiff = 0;
};

void register_key_up(Input* input, uint8 key, fptr_keyevent e) {
  input->keyUpEvents[key].emplace_front(e);
}

void register_key_down(Input* input, uint8 key, fptr_keyevent e) {
  input->keyDownEvents[key].emplace_front(e);
}

void unregister_key_up(Input* input, uint8 key, fptr_keyevent e) {
  input->keyUpEvents[key].remove(e);
}

void unregister_key_down(Input* input, uint8 key, fptr_keyevent e) {
  input->keyDownEvents[key].remove(e);
}

void trigger_events(const KeyList& list) {
  for(fptr_keyevent e : list) e();
}

Input* input_start() {
  return new Input; 
}

void input_end(Input* input) {
  for(uint8 key = 0; key < MAXKEYVALUE; ++key) {
    input->keyDownEvents[key].clear();
    input->keyUpEvents[key].clear();
  }
  delete input;
}

void input_tick(Input* input) {
  for(uint8 key = 0; key < MAXKEYVALUE; ++key) {
    bool& in = input->keyInput[key];
    bool& flag = input->keyFlags[key];

    if(in && !flag) {
      flag = 1;
      trigger_events(input->keyDownEvents[key]);
    }
    else if(!in && flag) {
      flag = 0;
      trigger_events(input->keyUpEvents[key]);
    }
  } 
}

void set_key_state(Input* input, uint8 key, bool down) {
  if(key >= MAXKEYVALUE) return;
  input->keyInput[key] = down;
}

bool is_down(Input* input, uint8 key) {
  return input->keyInput[key];
}

void set_mouse_moved(Input* input, int32 x, int32 y) {
  input->mouseX = x;
  input->mouseY = y;
 
  input->mouseXDiff = x - input->lastMouseX;
  input->mouseYDiff = y - input->lastMouseY;
  
  input->lastMouseX = x;
  input->lastMouseY = y;
}

void reset_mouse(Input* input, int32 toX, int32 toY) {
  input->mouseX = toX;
  input->mouseY = toY;
  input->mouseXDiff = 0;
  input->mouseYDiff = 0;
  input->lastMouseX = toX;
  input->lastMouseY = toY;
}

