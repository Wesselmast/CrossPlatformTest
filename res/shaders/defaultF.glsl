#version 330 core

in vec3 fragNormal;
out vec4 color;

uniform vec3 objColor;

void main() {
  vec3 ambientColor = vec3(0.2, 0.2, 0.2);
  vec3 lightColor = vec3(0.917, 0.937, 0.741);

  vec3 norm = normalize(fragNormal);
  vec3 lightDir = vec3(-0.2, 0.8, -0.9);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

  vec3 result = (diffuse + ambientColor) * objColor;
  color = vec4(result, 1.0);
}
