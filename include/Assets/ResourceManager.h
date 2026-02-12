#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <ext/glad/glad.h>
#include <ext/stb_image.h>
#include <ext/tiny_gltf.h>

class Texture
{
public:
    GLuint GetID() const { return _id; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    int GetChannels() const { return _channels; }

    Texture() = default;
    ~Texture()
    {
        if (_id)
            glDeleteTextures(_id);
    }

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture &&other) noexcept
        : _id(other.id), _width(other._width), _height(other._height), _channels(other._channels), _path(std::move(other._path))
    {
        other._id = 0;
    }

    Texture &operator=(Texture &&other) noexcept
    {
        if (this != &other)
        {
            if (_id)
                glDeleteTextures(1, _id);

            _id = other._id;
            _width = other._width;
            _height = other._height;
            _channels = other._channels;
            _path = std::move(other._path);
            other.id = 0;
        }
        return *this;
    }

private:
    std::string _path;
    GLuint _id = 0;
    int _width = 0;
    int _height = 0;
    int _channels = 0;
};

class ResourceManager
{
public:
    ResourceManager() { CreateDefaultTextures(); }
    ~ResourceManager() { CleanUp(); }

    std::shared_ptr<Texture> LoadTexture(const std::string &path);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> _textureCache;
    std::unordered_map<std::string, class GLTFModel *> _modelCache;
    std::mutex _mutex;

    GLuint _whiteTexture = 0;
    GLuint _blackTexture = 0;
    GLuint _normalTexture = 0;
};