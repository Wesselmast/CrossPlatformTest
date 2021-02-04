#pragma once
#include "Types.cpp"
#include "Math.cpp"

static uint32 perlin_seed = 0x332FF52D;

static float64 perlin_fade(float64 t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static float64 perlin_lerp(float64 a, float64 b, float64 t) {
  return (1.0 - t) * a + t * b;
}

static uint32 perlin_hash(uint32 x, uint32 y) {
  x  = (x << 16) | (y & 0xFFFF);
  x ^= x >> 16;
  x *= perlin_seed;
  x ^= x >> 16;
  x *= perlin_seed;
  x ^= x >> 16;
  return x;
}

static float64 perlin_grad(uint32 cx, uint32 cy, float64 dx, float64 dy) {
  float64 cosPI4 = cos(pi() / 4.0);
  float64 cosPI8 = cos(pi() / 8.0);
  float64 sinPI8 = sin(pi() / 8.0);

  switch(perlin_hash(cx, cy) & 0xF) {
    case 0x0: return  dx;
    case 0x1: return  dy;
    case 0x2: return -dx;
    case 0x3: return -dy;
    
    case 0x4: return  dx * cosPI4 +  dy * cosPI4;
    case 0x5: return -dx * cosPI4 +  dy * cosPI4;
    case 0x6: return -dx * cosPI4 + -dy * cosPI4;
    case 0x7: return  dx * cosPI4 + -dy * cosPI4;
    
    case 0x8: return  dx * cosPI8 +  dy * sinPI8;
    case 0x9: return  dx * sinPI8 +  dy * cosPI8;
    case 0xA: return -dx * sinPI8 +  dy * cosPI8;
    case 0xB: return -dx * cosPI8 +  dy * sinPI8;

    case 0xC: return -dx * cosPI8 + -dy * sinPI8;
    case 0xD: return -dx * sinPI8 + -dy * cosPI8;
    case 0xE: return  dx * sinPI8 + -dy * cosPI8;
    case 0xF: return  dx * cosPI8 + -dy * sinPI8;
  }
  return 0.0;
}

static float64 perlin_noise(float64 x, float64 y) {
  uint32 xr = (uint32)x;
  uint32 yr = (uint32)y;
  float64 xf = x - xr;
  float64 yf = y - yr;

  float64 grad00 = perlin_grad(xr    , yr    , xf      , yf);
  float64 grad10 = perlin_grad(xr + 1, yr    , xf - 1.0, yf);
  float64 grad01 = perlin_grad(xr    , yr + 1, xf      , yf - 1.0);
  float64 grad11 = perlin_grad(xr + 1, yr + 1, xf - 1.0, yf - 1.0);

  float64 fadeX = perlin_fade(xf);
  float64 fadeY = perlin_fade(yf);

  float64 lerp1 = perlin_lerp(grad00, grad10, fadeX);
  float64 lerp2 = perlin_lerp(grad01, grad11, fadeX);

  return 0.5 + perlin_lerp(lerp1, lerp2, fadeY) * sqrt1_2();
}

static float64 fractal_noise(float64 x, float64 y, uint32 octaves, float64 persistence) {
  float64 total = 0.0;
  float64 max   = 0.0;
  float64 freq  = 1.0;
  float64 ampl  = 1.0;

  for(uint32 i = 0; i < octaves; ++i) {
    total += perlin_noise(x * freq, y * freq) * ampl;
    max  += ampl;
    ampl *= persistence;
    freq *= 2;
  }

  return total / max;
}
