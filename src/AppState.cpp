#pragma once
#include "Actor.cpp"
#include "Asset.cpp"
#include "ScriptComponent.cpp"

struct AppState {
  ActorList  actors;
  AssetMap   aLUT;
  ScriptList scripts;

  Actor* camera;
};
