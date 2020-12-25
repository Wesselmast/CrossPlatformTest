#version 330 core

layout(location = 0) in vec2 vertexPosition;

uniform mat4 projection; 

void main() {
  gl_Position = projection * vec4(vertexPosition, 0.0, 1.0);
}
