#pragma once
#include "Object.h"
#include <memory>
#include <vector>

class Scene {
public:
    std::vector<std::shared_ptr<Object>> objects;

    std::shared_ptr<Object> createObject(const std::string& name, const Mesh& mesh) {
        auto obj = std::make_shared<Object>(name, mesh);
        objects.push_back(obj);
        return obj;
    }

    void draw(GLuint modelUniform) {
        for (auto& obj : objects)
            obj->draw(modelUniform);
    }
};

