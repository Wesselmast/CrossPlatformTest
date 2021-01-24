#version 330 core

#define MAX_LIGHTS 8

struct Light {
  vec3 color;
  vec3 position;
  float radius;
};

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragTexcoord;

out vec4 color;

uniform int renderUnlit;
uniform vec3 camPos;

uniform Light lights[MAX_LIGHTS];
uniform int amtOfLights;

//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;

uniform sampler2D diffuseMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D AOMap;
uniform sampler2D normalMap;

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

vec3 normal_from_map() {
  vec3 tangentNormal = texture(normalMap, fragTexcoord).xyz * 2.0 - 1.0;

  vec3 Q1  = dFdx(fragPos);
  vec3 Q2  = dFdy(fragPos);
  vec2 st1 = dFdx(fragTexcoord);
  vec2 st2 = dFdy(fragTexcoord);

  vec3 N   =  normalize(fragNormal);
  vec3 T   =  normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B   = -normalize(cross(N, T));
  mat3 TBN =  mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}

void main() {
  vec3 albedo     = texture(diffuseMap,   fragTexcoord).rgb;
  float metallic  = texture(metallicMap,  fragTexcoord).r;
  float roughness = texture(roughnessMap, fragTexcoord).r;
  float ao        = texture(AOMap, 	  fragTexcoord).r;

  vec3 N = normal_from_map();
  vec3 V = normalize(camPos - fragPos);

  float BM = max(metallic,  0.0000001);
  float BR = max(roughness, 0.0000001);
  
  vec3 env = albedo * texture(skybox, reflect(V, N)).rgb;
  vec3 R = mix(vec3(0.04), env, BM);

  vec3 Lo = vec3(0.0);
  for(int i = 0; i < amtOfLights; ++i) {
    vec3 L = normalize(lights[i].position - fragPos);
    vec3 H = normalize(V + L);
    float distance = length(lights[i].position - fragPos);
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

    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  }

  vec3 ambient = albedo * 0.03 * ao;
  vec3 lit = ambient + Lo;
  lit = lit / (lit + vec3(1.0));
  lit = pow(lit, vec3(1.0 / 2.2));

  lit = lit * (1.0 - renderUnlit) + renderUnlit;

  color = vec4(lit, 1.0);
}
