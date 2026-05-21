#version 330 core
#define MAX_BONES 128

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;
layout (location = 3) in ivec4 a_BoneIds;
layout (location = 4) in vec4 a_BoneWeights;

out vec2 v_UV;
out vec3 v_Normal;
out vec3 v_WorldPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_BoneMatrices[MAX_BONES]; // Массив матриц костей
uniform int u_Skinned;                  // 1 = скиннинг включён, 0 = статичная модель

void main() {
    vec4 pos = vec4(a_Position, 1.0);
    vec3 norm = a_Normal;

    if (u_Skinned == 1) {
        vec4 skinnedPos = vec4(0.0);
        vec3 skinnedNorm = vec3(0.0);
        for (int i = 0; i < 4; i++) {
            if (a_BoneWeights[i] > 0.0 && a_BoneIds[i] >= 0 && a_BoneIds[i] < MAX_BONES) {
                mat4 boneMat = u_BoneMatrices[a_BoneIds[i]];
                skinnedPos += boneMat * pos * a_BoneWeights[i];
                skinnedNorm += mat3(boneMat) * norm * a_BoneWeights[i];
            }
        }
        pos = skinnedPos;
        norm = normalize(skinnedNorm);
    }

    vec4 worldPos = u_Model * pos;
    v_WorldPos = worldPos.xyz;
    v_Normal = mat3(transpose(inverse(u_Model))) * norm;
    v_UV = a_UV;
    gl_Position = u_Projection * u_View * worldPos;
}
