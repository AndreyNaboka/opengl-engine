#include <glad/glad.h>

#include "Scene.h"

static void SetupMesh(float *vertices,
                      unsigned int *indices,
                      unsigned int vcount,
                      unsigned int icount,
                      unsigned int &vao,
                      unsigned int &indexCount)
{
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    indexCount = icount;
}

MeshRenderer Scene::CreateGroundMesh()
{
    float repeat = 50.0f; // подберите значение: 10, 20, 50...

    float vertices[] = {
        -50.0f,
        0.0f,
        -50.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f * repeat,
        0.0f * repeat,
        50.0f,
        0.0f,
        -50.0f,
        0.0f,
        1.0f,
        0.0f,
        1.0f * repeat,
        0.0f * repeat,
        50.0f,
        0.0f,
        50.0f,
        0.0f,
        1.0f,
        0.0f,
        1.0f * repeat,
        1.0f * repeat,
        -50.0f,
        0.0f,
        50.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f * repeat,
        1.0f * repeat,
    };
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    MeshRenderer mr;
    SetupMesh(vertices, indices, 32, 6, mr.VAO, mr.IndexCount);
    return mr;
}

void Scene::AddEntity(const Transform &tr, const MeshRenderer &mr)
{
    _entities.push_back({tr, mr});
}

void Scene::PreRender()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::Render(const glm::mat4 &view,
                   const glm::mat4 &projection,
                   const glm::vec3 &cameraPos,
                   const glm::vec3 &lightPos)
{
    for (auto &e : _entities)
    {
        e.MeshRenderer.Shader->Use();

        glm::mat4 model = e.Transform.GetModelMatrix();
        e.MeshRenderer.Shader->SetMat4("model", model);
        e.MeshRenderer.Shader->SetMat4("view", view);
        e.MeshRenderer.Shader->SetMat4("projection", projection);

        e.MeshRenderer.Shader->SetVec3("veiwPos", cameraPos);
        e.MeshRenderer.Shader->SetVec3("lightPos", lightPos);
        e.MeshRenderer.Shader->SetVec3("lightColor", glm::vec3(1.0f));
        e.MeshRenderer.Shader->SetVec3("objectColor", glm::vec3(1.0f));

        e.MeshRenderer.Texture->Bind();
        e.MeshRenderer.Shader->SetInt("material.diffuse", 0);

        glBindVertexArray(e.MeshRenderer.VAO);
        glDrawElements(GL_TRIANGLES, e.MeshRenderer.IndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}