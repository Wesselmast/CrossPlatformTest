#version 330 core

in vec3 fragNormal;
in vec3 fragPos;

out vec4 color;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform float lightRadius;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

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
  vec3 lightColor = vec3(0.917, 0.937, 0.741);
  
  vec3 N = normalize(fragNormal);
  vec3 V = normalize(camPos - fragPos);

  vec3 R = mix(vec3(0.04), albedo, metallic);

  vec3 L = normalize(lightPos - fragPos);
  vec3 H = normalize(V + L);
  float distance = length(lightPos - fragPos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightColor * attenuation * lightRadius;

  float NdotV = max(dot(N, V), 0.0000001);
  float NdotL = max(dot(N, L), 0.0000001);
  float NdotH = max(dot(N, H), 0.0);
  float HdotV = max(dot(H, V), 0.0);

  float D = distributionGGX(NdotH, roughness);
  float G = geometrySmith(NdotV, NdotL, roughness);
  vec3  F = freshnelSchlick(HdotV, R);

  vec3 specular = D * G * F;
  specular /= 4.0 * NdotV * NdotL;

  vec3 kD = vec3(1.0) - F;
  kD *= 1.0 - metallic;

  vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

  vec3 ambient = albedo * 0.03;
  vec3 lit = ambient + Lo;
  lit = lit / (lit + vec3(1.0));
  lit = pow(lit, vec3(1.0 / 2.2));

  color = vec4(lit, 1.0);
}
