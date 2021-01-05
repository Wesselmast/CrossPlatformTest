#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec3 fragPosLocal;

out vec4 color;

uniform vec3 lightPos;
uniform mat4 normalMat; 

void main() {
  vec3 lightColor = vec3(0.937, 0.937, 0.856);
  vec3 ambientColor = lightColor * 0.18;

  vec3 norm = normalize(mat3(normalMat) * fragNormal);
  vec3 lightDir = normalize(lightPos - fragPosLocal);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

  float steepness = 1.0 - normalize(fragNormal).y;

  vec3 grass = vec3(0.1490, 0.3254, 0.1686);
  vec3 stone = vec3(0.4196, 0.4431, 0.4941);
  vec3 ice   = vec3(1.0000, 1.0000, 1.0000);

  vec3 objColor;
  vec3 snowMix = mix(ice, stone, clamp(steepness / 0.185, 0.0, 1.0));

  if(fragPos.y > 0.8) objColor = snowMix;
  else if(fragPos.y < 0.55 && steepness < 0.03) objColor = grass;
  else if(fragPos.y < 0.75) objColor = stone;
  else objColor = mix(stone, snowMix, (fragPos.y - 0.75) / (0.8 - 0.75));

  vec3 result = (diffuse + ambientColor) * objColor;
  color = vec4(result, 1.0);
}
