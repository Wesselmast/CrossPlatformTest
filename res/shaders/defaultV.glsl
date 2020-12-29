#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 viewProj; 
uniform mat4 model; 

out vec3 fragNormal;

void main() {
  mat4 mvp = model * viewProj;
  gl_Position = vec4(position, 1.0) * mvp;
  fragNormal = normal;
}
