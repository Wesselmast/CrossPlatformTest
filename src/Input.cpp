#pragma once
#include "List.cpp"

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

  MAXKEYVALUE       = KEY_DELETE + 1
};  

#elif defined(__WINDOWS__)
#endif

typedef void(*fptr_keyevent)(OpenGLState*); //instead of an OpenGLState*, this will probably be a pointer to application state

struct Input {
  bool keyInput[MAXKEYVALUE];
  bool keyFlags[MAXKEYVALUE];
  List<fptr_keyevent> keyDownEvents[MAXKEYVALUE];
  List<fptr_keyevent> keyUpEvents[MAXKEYVALUE];
};

void register_key_up(Input* input, uint8 key, fptr_keyevent e) {
  input->keyUpEvents[key].insert(e);
}

void register_key_down(Input* input, uint8 key, fptr_keyevent e) {
  input->keyDownEvents[key].insert(e);
}

void unregister_key_up(Input* input, uint8 key, fptr_keyevent e) {
  input->keyUpEvents[key].remove(e);
}

void unregister_key_down(Input* input, uint8 key, fptr_keyevent e) {
  input->keyDownEvents[key].remove(e);
}

void trigger_events(const List<fptr_keyevent>& list, OpenGLState* state) {
  Node<fptr_keyevent>* current = list.head;
  while(current) {
    current->data(state);
    current = current->next;
  }
}

Input* input_start() {
  return (Input*)malloc(sizeof(Input));
}

void input_end(Input* input) {
  for(uint8 key = 0; key < MAXKEYVALUE; ++key) {
    input->keyDownEvents[key].free_list();
    input->keyUpEvents[key].free_list();
  }
  free(input);
}

void input_tick(Input* input, OpenGLState* state) {
  for(uint8 key = 0; key < MAXKEYVALUE; ++key) {
    bool& in = input->keyInput[key];
    bool& flag = input->keyFlags[key];

    if(in && !flag) {
      flag = 1;
      trigger_events(input->keyDownEvents[key], state);
    }
    else if(!in && flag) {
      flag = 0;
      trigger_events(input->keyUpEvents[key], state);
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
