#version 330 core
in vec3 v_TexCoords;
out vec4 o_Color;

uniform samplerCube u_Skybox;
uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
//uniform float u_Time;   // для динамических эффектов (опционально)

void main() {
    vec3 skyDir = normalize(v_TexCoords);
    vec4 texColor = texture(u_Skybox, skyDir);
    vec3 sunDir = normalize(-u_SunDirection);
    float sunAmount = max(dot(skyDir, sunDir), 0.0);
    float sunDisc = smoothstep(0.9975, 0.9995, sunAmount);
    float sunGlow = pow(sunAmount, 96.0) * 0.45;
    texColor.rgb += u_SunColor * sunGlow;
    texColor.rgb = mix(texColor.rgb, u_SunColor * 2.2, sunDisc);
    // Простой эффект мерцания звёзд (на основе яркости)
//    float twinkle = sin(texColor.r * 50.0 + u_Time) * 0.2;
//    texColor.rgb += vec3(twinkle);
    o_Color = texColor;
}
