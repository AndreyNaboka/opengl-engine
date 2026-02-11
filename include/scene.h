#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Shader.h"
#include "texture.h"

struct Transform
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::rotate(model, glm::radians(Rotation.x), {1, 0, 0});
        model = glm::rotate(model, glm::radians(Rotation.y), {0, 1, 0});
        model = glm::rotate(model, glm::radians(Rotation.z), {0, 0, 1});
        model = glm::scale(model, Scale);
        return model;
    }
};

struct MeshRenderer
{
    std::shared_ptr<Shader> Shader;
    std::shared_ptr<texture> Texture;
    unsigned int VAO = 0;
    unsigned int IndexCount = 0;
};

class Scene
{
public:
    Scene() { glEnable(GL_DEPTH_TEST); }
    void AddEntity(const Transform &t, const MeshRenderer &mr);
    void Render(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &cameraPos, const glm::vec3 &lightPos);
    void PreRender();

    MeshRenderer CreateGroundMesh();

private:
    struct Entity
    {
        Transform Transform;
        MeshRenderer MeshRenderer;
    };
    std::vector<Entity> _entities;
    std::string _name;
};
