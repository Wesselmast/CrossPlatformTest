#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec3 fragPosLocal;

out vec4 color;

void main() {
  vec3 lightColor = vec3(0.937, 0.937, 0.856);
  vec3 ambientColor = lightColor * 0.18;

  vec3 norm = normalize(fragNormal);
  vec3 lightDir = normalize(vec3(0.0, -8.0, 4.0) - fragPosLocal);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

  vec3 objColor;
  if(fragPos.y > 2.3) {
    objColor = vec3(1.0, 1.0, 1.0);
  }
  else if(fragPos.y > 0.6) {
    objColor = vec3(0.4196, 0.4431, 0.4941); 
  }
  else {
    objColor = vec3(0.149, 0.3254, 0.1686);
  }

  vec3 result = (diffuse + ambientColor) * objColor;
  color = vec4(result, 1.0);
}
