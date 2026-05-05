#version 330 core
out vec4 FragColor;
in vec2 v_UV;
in vec3 v_Normal;
in vec3 v_WorldPos;

uniform sampler2D u_Texture;
uniform vec3 u_CameraPos;

void main() {
	vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
	vec3 norm = normalize(v_Normal);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 ambient = vec3(0.15);
	vec3 diffuse = diff * vec3(0.85);

	vec3 texColor = texture(u_Texture, v_UV).rgb;
	vec3 result = (ambient + diffuse) * texColor;
	FragColor = vec4(result, 1.0);
}
