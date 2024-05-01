#include "Model.h"

Model::Model(std::string const &filePath, std::string const &infFile, unsigned int isovalue) 
    :isosurface(filePath, infFile, isovalue), slicer()
{
    data = isosurface.MarchingCube();
    center = isosurface.center;

    size_t vertSize = data.size() * sizeof(float);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize, data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

Model::Model(std::string const &filePath, std::string const &infile) 
    :isosurface(), slicer(filePath, infile)
{
    data = slicer.Slice();
    center = slicer.center;

    size_t vertSize = data.size() * sizeof(float);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize, data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenTextures(0, &texture3D_id);
    glBindTexture(GL_TEXTURE_3D, texture3D_id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, slicer.resolution.x, slicer.resolution.y, slicer.resolution.z, 0, GL_RGBA, GL_FLOAT, slicer.m_texture3DData.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    int domain = 256;
    glGenTextures(1, &texture1D_id);
    glBindTexture(GL_TEXTURE_1D, texture1D_id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, domain, 0, GL_RGBA, GL_FLOAT, slicer.m_texture1DData.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void Model::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, data.size() / 6);
}

void Model::DrawVolume() {
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, texture3D_id);
    glBindTexture(GL_TEXTURE_1D, texture1D_id);
    glDrawArrays(GL_TRIANGLES, 0, data.size() / 6);
}

void Model::reBindTexture(std::vector<float> const &newTexture) {
    slicer.m_texture1DData = newTexture;
    // int domain = 256;
    int domain = slicer.m_texture1DData.size() / sizeof(float);
    glGenTextures(1, &texture1D_id);
    glBindTexture(GL_TEXTURE_1D, texture1D_id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, domain, 0, GL_RGBA, GL_FLOAT, slicer.m_texture1DData.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, 0);
}