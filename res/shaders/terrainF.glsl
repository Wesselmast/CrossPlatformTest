#version 330 core

//in the future, terrain will just use the PBR material and get different textures being generated procedurally.

#define MAX_LIGHTS 8

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
uniform vec3 camPos;

uniform Light lights[MAX_LIGHTS];
uniform int amtOfLights;

uniform mat4 normalMat; 

uniform sampler2D grassMap;
uniform sampler2D rocksMap;
uniform sampler2D snowMap;

uniform samplerCube skybox;

const float PI = 3.14159265359;

/*
  The following code is heavily inspired from this wonderful video on PBR shaders by Brian Will:
  https://www.youtube.com/watch?v=5p0e7YNONr8
*/

float distributionGGX(float NdotH, float r) {
  float r4 = r * r * r * r;
  float d  = NdotH * NdotH * (r4 - 1.0) + 1.0;
  d = PI * d * d;
  return r4 / max(d, 0.0000001);
}

float geometry_smith(float NdotV, float NdotL, float r) {
  float ra = r + 1.0;
  float k = (ra * ra) / 8.0;
  float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
  float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
  return ggx1 * ggx2;
}

vec3 freshnel_schlick(float HdotV, vec3 reflectivity) {
  return reflectivity + (1.0 - reflectivity) * pow(1.0 - HdotV, 5.0);
}

void main() {
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

  vec3 N = normalize(mat3(normalMat) * fragNormal);
  vec3 V = normalize(camPos - fragPos);

  float BM = 0.0;
  float BR = 0.8;
  
  vec3 env = objColor * texture(skybox, reflect(V, N)).rgb;
  vec3 R = mix(vec3(0.04), env, BM);

  vec3 Lo = vec3(0.0);
  for(int i = 0; i < amtOfLights; ++i) {
    vec3 L = normalize(lights[i].position - fragPosLocal);
    vec3 H = normalize(V + L);
    float distance = length(lights[i].position - fragPosLocal);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lights[i].color * attenuation * lights[i].radius;

    float NdotV = max(dot(N, V), 0.0000001);
    float NdotL = max(dot(N, L), 0.0000001);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    float D = distributionGGX(NdotH, BR);
    float G = geometry_smith(NdotV, NdotL, BR);
    vec3  F = freshnel_schlick(HdotV, R);

    vec3 specular = D * G * F;
    specular /= 4.0 * NdotV * NdotL;

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - BM;

    Lo += (kD * objColor / PI + specular) * radiance * NdotL;
  }

  vec3 ambient = objColor * 0.03;
  vec3 lit = ambient + Lo;

  lit = lit / (lit + vec3(1.0));
  lit = pow(lit, vec3(1.0 / 2.2));

  lit = lit * (1.0 - renderUnlit) + renderUnlit;
  color = vec4(lit, 1.0);
}
