#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 viewProj; 
uniform mat4 model; 
uniform mat4 normalMat; 

out vec3 fragNormal;
out vec3 fragPos;
out vec2 fragTexcoord;

void main() {
  fragPos = vec3(vec4(position, 1.0) * model); 
  fragNormal = mat3(normalMat) * normal;
  fragTexcoord = texcoord;

  gl_Position = vec4(fragPos, 1.0) * viewProj;
}
