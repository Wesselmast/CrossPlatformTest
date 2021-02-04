#pragma once

#include "Components.cpp"
#include "Macros.cpp"
#include <unordered_map>
#include <forward_list>

struct Actor;
typedef std::forward_list<Actor*> ActorList;

struct Actor {
  ActorList* list;
  std::unordered_map<const char*, Component*> components;

  Actor(ActorList* list) : list(list) {
    list->push_front(this);
  }

  ~Actor() {
    for(auto itr : components) {
      if(itr.second) delete itr.second;
    }
    components.clear();
    list->remove(this);
  }
};

Actor* actor(ActorList& list) {
  return new Actor(&list);
}

void add_component(Actor* a, const char* name, Component* c) {
  a->components[name] = c;
}

template<typename T>
T get_component(Actor* a, const char* name) {
  auto itr = a->components.find(name);
  if(itr == a->components.end()) {
    log_("ERR: Component name wasn't found!\n");
    return nullptr;
  }
  return (T)itr->second;
}
