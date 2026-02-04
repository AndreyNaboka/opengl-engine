#pragma once

#include <string>
#include <vector>

class render_object;

class scene
{
public:
    explicit scene(const std::string &name) { _name = name; }
    void add_render_object(render_object *obj) { _render_objects_list.emplace_back(obj); }
    void pre_render();
    void render();

private:
    std::string _name;
    std::vector<render_object *> _render_objects_list;
};