#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 viewProj; 
uniform mat4 model; 

void main() {
  mat4 mvp = model * viewProj;
  gl_Position = vec4(vertexPosition, 1.0) * mvp;
}
