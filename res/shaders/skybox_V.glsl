#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 texCoords;

void main() {
  texCoords = position;
  vec4 pos = vec4(position, 1.0) * mat4(mat3(view)) * projection;
  gl_Position = pos.xyww;
}
