#pragma once
#include "Actor.cpp"
#include "Asset.cpp"

struct Light;
struct Mesh;
struct Camera;

struct AppState {
  ActorList actors;
  AssetMap  aLUT;

  Light* light;
  Camera* camera;
};
