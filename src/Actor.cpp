#pragma once

#include "Components.cpp"
#include <unordered_map>

struct Actor {
  std::unordered_map<const char*, Component*> components;

  void tick(OpenGLState* state) {
    for(auto pair : components) {
      pair.second->tick(state);
    }
  }

  ~Actor() {
    for(auto itr : components) {
      delete itr.second;
    }
    components.clear();
  }
};

Actor* actor(OpenGLState* state) {
  Actor* a = new Actor;
  state->actors.push_front(a);
  return a;
}

void add_component(Actor* a, const char* name, Component* c) {
  a->components[name] = c;
}

template<typename T>
void get_component(Actor* a, const char* name, T outC) {
  //ADD ERROR CHECKING
  outC = (T)a->components[name];
}

void destroy(OpenGLState* state, Actor* actor) {
  state->actors.remove(actor);
  delete actor;
}
