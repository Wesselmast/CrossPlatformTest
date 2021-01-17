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

out vec4 color;

uniform int renderUnlit;

uniform Light lights[MAX_LIGHTS];
uniform int amtOfLights;

uniform mat4 normalMat; 

const float PI = 3.14159265359;

/*
  The following code is heavily inspired from this wonderful video on PBR shaders by Brian Will:
  https://www.youtube.com/watch?v=5p0e7YNONr8
*/

float distributionGGX(float NdotH, float roughness) {
  float r4 = roughness * roughness * roughness * roughness;
  float d  = NdotH * NdotH * (r4 - 1.0) + 1.0;
  d = PI * d * d;
  return r4 / max(d, 0.0000001);
}

float geometrySmith(float NdotV, float NdotL, float roughness) {
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;
  float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
  float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
  return ggx1 * ggx2;
}

vec3 freshnelSchlick(float HdotV, vec3 reflectivity) {
  return reflectivity + (1.0 - reflectivity) * pow(1.0 - HdotV, 5.0);
}

void main() {
  vec3 norm = normalize(mat3(normalMat) * fragNormal);
  vec3 lightDir = normalize(lights[0].position - fragPosLocal);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * lights[0].color;

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

  vec3 lit = (diffuse + lights[0].color * 0.04) * objColor;

  lit = lit / (lit + vec3(1.0));
  lit = pow(lit, vec3(1.0 / 2.2));

  lit = lit * (1.0 - renderUnlit) + renderUnlit;
  color = vec4(lit, 1.0);
}
