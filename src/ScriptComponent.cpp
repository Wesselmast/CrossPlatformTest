#pragma once

#include "Types.cpp"
#include "Components.cpp"

struct Input;
struct Actor;

typedef void(*fptr_scripttick) (Input*, Actor*, float64, float64);
typedef void(*fptr_scriptstart)(Input*, Actor*);

struct ScriptComponent;
typedef std::forward_list<ScriptComponent*> ScriptList;

struct ScriptComponent : public Component {
  fptr_scripttick  tickf;
  Actor* actor;

  void tick(Input* input, float64 dt, float64 time) {
    tickf(input, actor, dt, time);
  }

  ScriptComponent(ScriptList& list) {
    list.emplace_front(this);
  }
}; 

ScriptComponent* component_script(ScriptList& list, Actor* actor, fptr_scripttick tick) {
  ScriptComponent* sc = new ScriptComponent(list);
  sc->actor = actor;
  sc->tickf = tick;
  return sc;
}
