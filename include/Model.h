#pragma once
#include "Isosurface.h"
#include "Slicing.h"
#include <glad/glad.h>

class Model {
public:
    Model(std::string const &filePath, std::string const &infFile, unsigned int isovalue);
    Model(std::string const &filePath, std::string const &infile); //volume rendering
    GLuint vbo;
    glm::vec3 center;
    void draw();
    void DrawVolume();
    void reBindTexture(std::vector<float> const &newTexture);
private:
    Isosurface isosurface;
    Slicer slicer;
    std::vector<float> data;
    GLuint vao;
    unsigned int texture3D_id;
    unsigned int texture1D_id;
};