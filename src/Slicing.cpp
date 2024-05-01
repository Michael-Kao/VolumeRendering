#include "Slicing.h"


Slicer::Slicer(std::string const &filePath, std::string const &infFile) 
    : m_data(filePath, infFile) {
    m_data.Read();
    center.x = m_data.resolution.x * m_data.ratio.x * 0.5f;
    center.y = m_data.resolution.y * m_data.ratio.y * 0.5f;
    center.z = m_data.resolution.z * m_data.ratio.z * 0.5f;
    resolution = m_data.resolution;
    GenerateTexture3DData();
    GenerateTexture1DData();
}

std::vector<float> Slicer::Slice() {
    std::vector<float> res;
    int xmax = m_data.resolution.x;
    int ymax = m_data.resolution.y;
    int zmax = m_data.resolution.z;
    float xmin = 0;
    float ymin = 0;
    float zmin = 0;

    int sampleRate = 8;
    //back to front
    // for(int x = sampleRate * (xmax - 1); x >= 0; --x) {
    //     float xCoord = x / (float)sampleRate;
    //     float s = xCoord / xmax;
    //     std::vector<float> pData;
    //     // {coord, texture coord}
    //     // lbtn -> rbtn -> rtop -> ltop -> lbtn -> rtop
    //     pData = {xCoord, ymin, zmin, s, 0, 0};
    //     res.insert(res.end(), pData.begin(), pData.end());
    //     pData = {xCoord, (float)(ymax - 1), zmin, s, 1, 0};
    //     res.insert(res.end(), pData.begin(), pData.end());
    //     pData = {xCoord, (float)(ymax - 1), (float)(zmax - 1), s, 1, 1};
    //     res.insert(res.end(), pData.begin(), pData.end());

    //     pData = {xCoord, ymin, (float)(zmax - 1), s, 0, 1};
    //     res.insert(res.end(), pData.begin(), pData.end());
    //     pData = {xCoord, ymin, zmin, s, 0, 0};
    //     res.insert(res.end(), pData.begin(), pData.end());
    //     pData = {xCoord, (float)(ymax - 1), (float)(zmax - 1), s, 1, 1};
    //     res.insert(res.end(), pData.begin(), pData.end());
    // }
    for(int z = sampleRate * (zmax - 1); z >= 0; --z) {
        float zCoord = z / (float)sampleRate;
        float t = zCoord / zmax;
        std::vector<float> pData;
        // {coord, texture coord}
        // lbtn -> rbtn -> rtop -> ltop -> lbtn -> rtop
        pData = {xmin, ymin, zCoord, 0, 0, t};
        res.insert(res.end(), pData.begin(), pData.end());
        pData = {(float)(xmax - 1), ymin, zCoord, 1, 0, t};
        res.insert(res.end(), pData.begin(), pData.end());
        pData = {(float)(xmax - 1), (float)(ymax - 1), zCoord, 1, 1, t};
        res.insert(res.end(), pData.begin(), pData.end());

        pData = {xmin, (float)(ymax - 1), zCoord, 0, 1, t};
        res.insert(res.end(), pData.begin(), pData.end());
        pData = {xmin, ymin, zCoord, 0, 0, t};
        res.insert(res.end(), pData.begin(), pData.end());
        pData = {(float)(xmax - 1), (float)(ymax - 1), zCoord, 1, 1, t};
        res.insert(res.end(), pData.begin(), pData.end());
    }

    // bind this res into a vbo
    return res;
}

void Slicer::CalcGradient() {
    // front to back
    int xsize = m_data.resolution.x;
    int ysize = m_data.resolution.y;
    int zsize = m_data.resolution.z;

    gradients.reserve(xsize * ysize * zsize);

    for(int z = 0; z < zsize; ++z) {
        for(int y = 0; y < ysize; ++y) {
            for(int x = 0; x < xsize; ++x) {
                float xDiff = 0, yDiff = 0, zDiff = 0;
                if (x == 0) {
                    xDiff = ForwardDifference(m_data.Value(0, y, z), m_data.Value(1, y, z));
                } else if (x == xsize - 1) {
                    xDiff = BackwardDifference(m_data.Value(x, y, z), m_data.Value(x - 1, y, z));
                } else {
                    xDiff = CentralDifference(m_data.Value(x - 1, y, z), m_data.Value(x + 1, y, z));
                }

                if (y == 0) {
                    yDiff = ForwardDifference(m_data.Value(x, 0, z), m_data.Value(x, 1, z));
                } else if (y == ysize - 1) {
                    yDiff = BackwardDifference(m_data.Value(x, y, z), m_data.Value(x, y - 1, z));
                } else {
                    yDiff = CentralDifference(m_data.Value(x, y - 1, z), m_data.Value(x, y + 1, z));
                }

                if (z == 0) {
                    zDiff = ForwardDifference(m_data.Value(x, y, 0), m_data.Value(x, y, 1));
                } else if (z == zsize - 1) {
                    zDiff = BackwardDifference(m_data.Value(x, y, z), m_data.Value(x, y, z - 1));
                } else {
                    zDiff = CentralDifference(m_data.Value(x, y, z - 1), m_data.Value(x, y, z + 1));
                }

                if(xDiff == 0 && yDiff == 0 && zDiff == 0) {
                    gradients.push_back({0.f, 0.f, 0.f});
                } else {
                    gradients.push_back(glm::normalize(glm::vec3{xDiff, yDiff, zDiff}));
                }
            }
        }
    }
}

float Slicer::CentralDifference(float front, float back) {
    return (front - back) / 2;
}

float Slicer::ForwardDifference(float self, float front) {
    return front - self;
}

float Slicer::BackwardDifference(float self, float back) {
    return self - back;
}

glm::vec3 Slicer::gValue(glm::vec3 coord) {
    return gValue(coord.x, coord.y, coord.z);
}

glm::vec3 Slicer::gValue(int x, int y, int z) {
    return gradients[z * m_data.resolution.x * m_data.resolution.y + y * m_data.resolution.x + x];
}

void Slicer::GenerateTexture3DData() {
    if(gradients.empty())
        CalcGradient();
    
    float maxIntensity = m_data.getMaxVal();
    
    for(int i = 0; i < gradients.size(); ++i) {
        glm::vec3 g = gradients[i];
        float val = m_data.Value(i) / maxIntensity;
        std::vector<float> temp = {g.x, g.y, g.z, val};
        m_texture3DData.insert(m_texture3DData.end(), temp.begin(), temp.end());
    }
}

void Slicer::GenerateTexture1DData() {
    float domain = 256;
    
    for(int i = 0; i < domain; ++i) {
        float alpha = (float)i / domain;
        std::vector<float> temp = {0.3, 0.3, 0.7, alpha};
        if(alpha < 0.3)
            temp = {0.0, 0.0, 0.0, alpha};
        m_texture1DData.insert(m_texture1DData.end(), temp.begin(), temp.end());
    }
}