#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 viewProj; 
uniform mat4 model; 
uniform mat4 normalMat; 

out vec3 fragNormal;
out vec3 fragPosLocal;
out vec3 fragPos;

void main() {
  fragPosLocal = vec3(vec4(position, 1.0) * model); 
  fragPos = position;
  fragNormal = mat3(normalMat) * normal;

  gl_Position = vec4(fragPosLocal, 1.0) * viewProj;
}
