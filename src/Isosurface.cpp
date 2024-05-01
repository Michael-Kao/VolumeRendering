#include "Isosurface.h"

Isosurface::Isosurface(std::string const &filePath, std::string const &infFile, unsigned int isovalue)
    : m_data(filePath, infFile), isovalue(isovalue) {
    m_data.Read();
    center.x = m_data.resolution.x * m_data.ratio.x * 0.5f;
    center.y = m_data.resolution.y * m_data.ratio.y * 0.5f;
    center.z = m_data.resolution.z * m_data.ratio.z * 0.5f;
}

void Isosurface::CalcGradient() {
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

std::vector<float> Isosurface::MarchingCube() {
    if(gradients.empty())
        CalcGradient();
    
    std::vector<float> res;

    int xsize = m_data.resolution.x;
    int ysize = m_data.resolution.y;
    int zsize = m_data.resolution.z;

    std::vector< std::vector<int> > 
    pattern{
        {0, 0, 0},
        {1, 0, 0},
        {1, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 1, 0},
        {1, 1, 1},
        {0, 1, 1}
    };

    for(int z = 0; z < zsize - 1; ++z) {
        for(int y = 0; y < ysize - 1; ++y) {
            for(int x = 0; x < xsize - 1; ++x) {
                //set current cube index
                std::vector<glm::vec3> cellIdx;
                for(int i = 0; i < 8; ++i) {
                    cellIdx.push_back(
                        {x + pattern[i][0], y + pattern[i][1], z + pattern[i][2]}
                    );
                }

                //find the case number
                int status = 0;
                for(int i = 0; i < 8; ++i) {
                    if(m_data.Value(cellIdx[i]) < isovalue)
                        status |= (1 << i);
                }

                auto intersectedEdge = cube::kCubeEdges[status];
                
                if (intersectedEdge == 0)
                    continue;

                // 12 edges, each vert has info about (coordinate, gradient)
                std::vector< std::vector<float> > vertOnEdge(12, std::vector<float>(6, 0));
                
                // get isosurface vertex info
                if (intersectedEdge & (1 << 0)) 
                    vertOnEdge[0] = Interpolate(cellIdx[0], cellIdx[1]);
                if (intersectedEdge & (1 << 1)) 
                    vertOnEdge[1] = Interpolate(cellIdx[1], cellIdx[2]);
                if (intersectedEdge & (1 << 2)) 
                    vertOnEdge[2] = Interpolate(cellIdx[2], cellIdx[3]);
                if (intersectedEdge & (1 << 3)) 
                    vertOnEdge[3] = Interpolate(cellIdx[3], cellIdx[0]);
                if (intersectedEdge & (1 << 4)) 
                    vertOnEdge[4] = Interpolate(cellIdx[4], cellIdx[5]);
                if (intersectedEdge & (1 << 5)) 
                    vertOnEdge[5] = Interpolate(cellIdx[5], cellIdx[6]);
                if (intersectedEdge & (1 << 6)) 
                    vertOnEdge[6] = Interpolate(cellIdx[6], cellIdx[7]);
                if (intersectedEdge & (1 << 7)) 
                    vertOnEdge[7] = Interpolate(cellIdx[7], cellIdx[4]);
                if (intersectedEdge & (1 << 8)) 
                    vertOnEdge[8] = Interpolate(cellIdx[0], cellIdx[4]);
                if (intersectedEdge & (1 << 9)) 
                    vertOnEdge[9] = Interpolate(cellIdx[1], cellIdx[5]);
                if (intersectedEdge & (1 << 10)) 
                    vertOnEdge[10] = Interpolate(cellIdx[2], cellIdx[6]);
                if (intersectedEdge & (1 << 11)) 
                    vertOnEdge[11] = Interpolate(cellIdx[3], cellIdx[7]);
                
                for(int i = 0; cube::kCubeTriangles[status][i] != -1; ++i) {
                    int edge = cube::kCubeTriangles[status][i];
                    res.insert(res.end(), vertOnEdge[edge].begin(), vertOnEdge[edge].end());
                }
            }
        }
    }
    return res;
}

std::vector<float>
Isosurface::Interpolate(glm::vec3 v1, glm::vec3 v2) {
    float epsilon = 0.0001;
    std::vector<float> value{m_data.Value(v1), m_data.Value(v2)};
    float ratio = (isovalue - value[0]) / (value[1] - value[0]);

    std::vector<glm::vec3> g{gValue(v1), gValue(v2)};
    std::vector<glm::vec3> pointPos{v1 * m_data.ratio, v2 * m_data.ratio};

    if (std::abs(value[0] - value[1]) < epsilon) {
        // 差距太小取中間
        glm::vec3 vert = (pointPos[0] + pointPos[1]) * 0.5f;
        glm::vec3 grad = glm::normalize((g[0] + g[1]) * 0.5f);
        return {vert.x, vert.y, vert.z, grad.x, grad.y, grad.z};
    }
    if (std::abs(isovalue - value[0]) < epsilon) {
        // 距離v1太近取v1
        return {pointPos[0].x, pointPos[0].y, pointPos[0].z, g[0].x, g[0].y, g[0].z};
    }
    if (std::abs(isovalue - value[1]) < epsilon) {
        return {pointPos[1].x, pointPos[1].y, pointPos[1].z, g[1].x, g[1].y, g[1].z};
    }

    glm::vec3 vert = ratio * (pointPos[1] - pointPos[0]) + pointPos[0];
    glm::vec3 grad = glm::normalize(ratio * (g[1] - g[0]) + g[0]);
    return {vert.x, vert.y, vert.z, grad.x, grad.y, grad.z};
}

float Isosurface::CentralDifference(float front, float back) {
    return (back - front) / 2;
}

float Isosurface::BackwardDifference(float self, float back) {
    return self - back;
}

float Isosurface::ForwardDifference(float self, float front) {
    return front - self;
}

glm::vec3 Isosurface::gValue(glm::vec3 coord) {
    int idx = coord.x + coord.y * m_data.resolution.x + coord.z * m_data.resolution.x * m_data.resolution.y;
    return gradients[idx];
}

glm::vec3 Isosurface::gValue(int x, int y, int z) {
    int idx = x + y * m_data.resolution.x + z * m_data.resolution.x * m_data.resolution.y;
    return gradients[idx];
}