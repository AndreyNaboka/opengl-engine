#version 330 core
out vec4 FragColor;
in vec2 v_UV;
in vec3 v_Normal;
in vec3 v_WorldPos;

uniform sampler2D u_Texture;
uniform samplerCube u_FogSkybox;
uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform float u_AmbientStrength;

layout(std140) uniform Camera {
  mat4 u_View;
  mat4 u_Projection;
  vec4 u_CameraPos;
};

void main() {
  vec3 norm = normalize(v_Normal);
  vec3 sunToSurface = normalize(-u_SunDirection);
  vec3 viewDir = normalize(u_CameraPos.xyz - v_WorldPos);

  float diff = max(dot(norm, sunToSurface), 0.0);
  float halfLambert = diff * 0.5 + 0.5;
  vec3 ambient = vec3(u_AmbientStrength);
  vec3 diffuse = u_SunColor * halfLambert * 0.92;
  vec3 reflectDir = reflect(u_SunDirection, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * diff;
  vec3 specular = u_SunColor * spec * 0.22;

  // Soft distance fog that fades into the actual skybox color.
  float dist = distance(u_CameraPos.xyz, v_WorldPos);
  float fogStart = 18.0;
  float fogDensity = 0.018;
  float fogDistance = max(dist - fogStart, 0.0);
  float fogFactor = 1.0 - exp(-fogDistance * fogDensity);
  fogFactor = smoothstep(0.0, 1.0, clamp(fogFactor, 0.0, 1.0));

  vec3 texColor = texture(u_Texture, v_UV).rgb;
  vec3 litColor = (ambient + diffuse) * texColor + specular;
  vec3 skyDir = normalize(v_WorldPos - u_CameraPos.xyz);
  vec3 fogColor = textureLod(u_FogSkybox, skyDir, 3.0).rgb;

  FragColor = vec4(mix(litColor, fogColor, fogFactor), 1.0);
}
