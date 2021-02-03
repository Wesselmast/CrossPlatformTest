#version 330 core

//in the future, terrain will just use the PBR material and get different textures being generated procedurally.

#define MAX_LIGHTS 1

struct Light {
  vec3 color;
  vec3 position;
  float radius;
};

in vec3 fragNormal;
in vec3 fragPos;
in vec3 fragPosLocal;
in vec2 fragTexcoord;

out vec4 color;

uniform int renderUnlit;

uniform Light lights[MAX_LIGHTS];
uniform int amtOfLights;

uniform mat4 normalMat; 

uniform sampler2D grassMap;
uniform sampler2D rocksMap;
uniform sampler2D snowMap;

void main() {
  vec3 norm = normalize(mat3(normalMat) * fragNormal);
  vec3 lightDir = normalize(lights[0].position - fragPosLocal);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lights[0].color;

  float steepness = 1.0 - normalize(fragNormal).y;

  vec3 grass = texture(grassMap, fragTexcoord).rgb; 
  vec3 stone = texture(rocksMap, fragTexcoord).rgb; 
  vec3 ice   = texture(snowMap,  fragTexcoord).rgb; 

  vec3 objColor;
  vec3 snowMix = mix(ice, stone, clamp(steepness / 0.085, 0.0, 1.0));
  vec3 grassMix = mix(grass, stone, clamp(steepness / 0.028, 0.0, 1.0));

  float snowLevel = 0.7;
  float rockLevel = snowLevel - 0.05;
  float grassLevel = rockLevel - 0.10;
  if(fragPos.y > snowLevel) objColor = snowMix;
  else if(fragPos.y < grassLevel) objColor = grassMix;
  else if(fragPos.y < rockLevel) objColor = stone;
  else objColor = mix(stone, snowMix, (fragPos.y - rockLevel) / (snowLevel - rockLevel));

  vec3 lit = (diffuse + lights[0].color * 0.04) * objColor;

  lit = lit / (lit + vec3(1.0));
  lit = pow(lit, vec3(1.0 / 2.2));

  lit = lit * (1.0 - renderUnlit) + renderUnlit;
  color = vec4(lit, 1.0);
}
