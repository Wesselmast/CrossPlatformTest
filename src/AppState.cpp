#pragma once
#include "Actor.cpp"
#include "Asset.cpp"

struct AppState {
  ActorList actors;
  AssetMap  aLUT;

  Actor* camera;
  Actor* movingLight;
};
