#pragma once

#define LAYOUT_POSITION_3D 0x01
#define LAYOUT_POSITION_2D 0x02
#define LAYOUT_NORMAL      0x04
#define LAYOUT_TEXCOORD    0x08
#define LAYOUT_COLOR       0x10

inline int32 get_layout_size(uint8 layout) {
  int32 size = 0;

  size += bool(layout & LAYOUT_POSITION_3D) * 3;
  size += bool(layout & LAYOUT_POSITION_2D) * 2;
  size += bool(layout & LAYOUT_NORMAL)      * 3;
  size += bool(layout & LAYOUT_TEXCOORD)    * 2;
  size += bool(layout & LAYOUT_COLOR)       * 3;

  return size * sizeof(float32);
}

inline void activate_layout_element(uint8& pos, int64& stride, uint8 totalSize, uint8 elementSize) {
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, elementSize, GL_FLOAT, GL_FALSE, totalSize, (void*)stride);
  stride += elementSize * sizeof(float32);
  ++pos;
} 

inline void activate_layout(uint8 layout) {
  int32 size = get_layout_size(layout);
  uint8 pos = 0;
  int64 stride = 0;

  if(layout & LAYOUT_POSITION_3D) activate_layout_element(pos, stride, size, 3);
  if(layout & LAYOUT_POSITION_2D) activate_layout_element(pos, stride, size, 2);
  if(layout & LAYOUT_NORMAL)      activate_layout_element(pos, stride, size, 3);
  if(layout & LAYOUT_TEXCOORD)    activate_layout_element(pos, stride, size, 2);
  if(layout & LAYOUT_COLOR)       activate_layout_element(pos, stride, size, 3);
}

