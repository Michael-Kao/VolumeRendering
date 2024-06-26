#pragma once
#include "VolumeData.h"
#include "MarchingCubeTable.h"

class Isosurface {
public:
    Isosurface(std::string const &filePath, std::string const &infFile, unsigned int isovalue);
    Isosurface() {}// do nothing
    std::vector<float> MarchingCube();
    glm::vec3 center;
private:
    VolumeData m_data;
    unsigned int isovalue;
    std::vector<glm::vec3> gradients;
    std::vector<float> Interpolate(glm::vec3 v1, glm::vec3 v2);
    void CalcGradient();
    float CentralDifference(float front, float back);
    float ForwardDifference(float self, float front);
    float BackwardDifference(float self, float back);
    glm::vec3 gValue(glm::vec3 coord);
    glm::vec3 gValue(int x, int y, int z);
};