#pragma once
#include "VolumeData.h"

class Slicer {
public:
    Slicer(std::string const &filePath, std::string const &infFile);
    Slicer() {} //do nothing
    std::vector<float> Slice();
    glm::vec3 center;
    glm::ivec3 resolution;
    std::vector<float> m_texture3DData;
    std::vector<float> m_texture1DData;
private:
    VolumeData m_data;
    std::vector<glm::vec3> gradients;
    void CalcGradient();
    float CentralDifference(float front, float back);
    float ForwardDifference(float self, float front);
    float BackwardDifference(float self, float back);
    void GenerateTexture1DData();
    void GenerateTexture3DData();
    glm::vec3 gValue(glm::vec3 coord);
    glm::vec3 gValue(int x, int y, int z);
};