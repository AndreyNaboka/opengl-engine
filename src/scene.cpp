#include <glad/glad.h>

#include "scene.h"

static void setup_mesh(float *vertices,
                       unsigned int *indices,
                       unsigned int vcount,
                       unsigned int icount,
                       unsigned int &vao,
                       unsigned int &index_count)
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
    index_count = icount;
}

mesh_renderer scene::create_ground_mesh()
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
    mesh_renderer mr;
    setup_mesh(vertices, indices, 32, 6, mr.vao, mr.index_count);
    return mr;
}

void scene::add_entity(const transform &tr, const mesh_renderer &mr)
{
    _entities.push_back({tr, mr});
}

void scene::pre_render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void scene::render(const glm::mat4 &view,
                   const glm::mat4 &projection,
                   const glm::vec3 &camera_pos,
                   const glm::vec3 &light_pos)
{
    for (auto &e : _entities)
    {
        e.mr.shader->use();

        glm::mat4 model = e.t.get_model_matrix();
        e.mr.shader->set_mat4("model", model);
        e.mr.shader->set_mat4("view", view);
        e.mr.shader->set_mat4("projection", projection);

        e.mr.shader->set_vec3("veiwPos", camera_pos);
        e.mr.shader->set_vec3("lightPos", light_pos);
        e.mr.shader->set_vec3("lightColor", glm::vec3(1.0f));
        e.mr.shader->set_vec3("objectColor", glm::vec3(1.0f));

        e.mr.texture->bind();
        e.mr.shader->set_int("material.diffuse", 0);

        glBindVertexArray(e.mr.vao);
        glDrawElements(GL_TRIANGLES, e.mr.index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}