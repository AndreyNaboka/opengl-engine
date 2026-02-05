#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "shader.h"
#include "texture.h"

struct transform
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    glm::mat4 get_model_matrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), {1, 0, 0});
        model = glm::rotate(model, glm::radians(rotation.y), {0, 1, 0});
        model = glm::rotate(model, glm::radians(rotation.z), {0, 0, 1});
        model = glm::scale(model, scale);
        return model;
    }
};

struct mesh_renderer
{
    std::shared_ptr<shader> shader;
    std::shared_ptr<texture> texture;
    unsigned int vao = 0;
    unsigned int index_count = 0;
};

class scene
{
public:
    void add_entity(const transform &t, const mesh_renderer &mr);
    void render(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_pos, const glm::vec3 &light_pos);
    void pre_render();

private:
    struct entity
    {
        transform t;
        mesh_renderer mr;
    };
    std::vector<entity> _entities;
    std::string _name;
};

mesh_renderer create_ground_mesh();
mesh_renderer create_cubes_mesh();