#version 330 core
out vec4 FragColor;
in vec2 v_UV;
in vec3 v_Normal;
in vec3 v_WorldPos;

uniform sampler2D u_Texture;
uniform vec3 u_CameraPos;

void main() {
  vec3 viewDir = normalize(u_CameraPos - v_WorldPos);
  vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
  vec3 norm = normalize(v_Normal);
    
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 ambient = vec3(0.15);
  vec3 diffuse = diff * vec3(0.85);
 
  // Простой туман на основе расстояния до камеры
  float dist = distance(u_CameraPos, v_WorldPos);
  float fogFactor = clamp(dist / 80.0, 0.0, 1.0);
 
  vec3 texColor = texture(u_Texture, v_UV).rgb;
  vec3 litColor = (ambient + diffuse) * texColor;
  vec3 fogColor = vec3(0.65, 0.75, 0.9); // Цвет неба из glClearColor
 
  FragColor = vec4(mix(litColor, fogColor, fogFactor), 1.0);
}
