#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 viewProj; 
uniform mat4 model; 

out vec3 fragNormal;
out vec3 fragPos;

void main() {
  fragPos = vec3(vec4(position, 1.0) * model); 
  gl_Position = vec4(fragPos, 1.0) * viewProj;
  fragNormal = mat3(transpose(inverse(model))) * normal; //inefficient, change to CPU later
}
