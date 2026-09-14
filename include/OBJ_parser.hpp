#pragma once

#include <vector>
#include <fstream>
#include <sstream>

#include <shader.hpp>
#include <model.hpp>

class VertexComponent{
public:
    std::vector<float> pos;
};

unsigned int count_nr_substr(const std::string member, const std::string habitat);

void loadOBJ(const std::string filepath, Model* modelin);