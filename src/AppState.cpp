#pragma once
#include "Actor.cpp"
#include "Asset.cpp"

struct AppState {
  ActorList actors;
  AssetMap  aLUT;

  Actor* rotatingCube;
  Actor* camera;
  Actor* movingLight;
};
