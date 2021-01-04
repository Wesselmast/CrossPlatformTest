#version 330 core

in vec3 fragNormal;
in vec3 fragPos;

out vec4 color;

uniform vec3 objColor;
uniform vec3 lightPos;

void main() {
  vec3 lightColor = vec3(0.917, 0.937, 0.741);
  vec3 ambientColor = lightColor * 0.28;

  vec3 norm = normalize(fragNormal);
  vec3 lightDir = normalize(lightPos - fragPos);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

  vec3 result = (diffuse + ambientColor) * objColor;
  color = vec4(result, 1.0);
}
