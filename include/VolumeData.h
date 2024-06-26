#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include "Util.h"

struct InfoKey {
    std::string static const Inf;
    std::string static const Raw;
    std::string static const Resolution;
    std::string static const VolumeSize;
    std::string static const vDataType;
    std::string static const Endian;
};

struct kAlias {
    std::unordered_set<std::string> static const Raw;
    std::unordered_set<std::string> static const Resolution;
    std::unordered_set<std::string> static const VolumeSize;
    std::unordered_set<std::string> static const vDataType;
    std::unordered_set<std::string> static const Endian;
};

class VolumeData {
public:
    VolumeData(std::string const &filePath, std::string const &infFile);
    VolumeData() {}// do nothing
    void Read();
    glm::ivec3 resolution;
    glm::vec3 ratio;
    float getMaxVal();
    float Value(int x, int y, int z);
    float Value(int idx);
    float Value(glm::ivec3 coordinate);
private:
    std::unordered_map<std::string, std::string> info;
    std::vector<float> scalar_field;
};