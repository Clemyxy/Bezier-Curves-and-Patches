#pragma once

#include "vec.h"
#include "mat.h"
#include "mesh.h"
#include <utility>
#include <vector>

class BezierSurface {
private:
    std::vector<std::vector<Point>> ctrlPts;

    //For analytical calculations
    const float cnp[16][16] = {
            {1, 0,  0,   0,   0,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 1,  0,   0,   0,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 2,  1,   0,   0,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 3,  3,   1,   0,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 4,  6,   4,   1,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 5,  10,  5,   1,    0,    0,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 6,  15,  20,  15,   6,    1,    0,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 7,  21,  35,  35,   21,   7,    1,    0,    0,    0,    0,    0,   0,   0,  0},
            {1, 8,  28,  56,  70,   56,   28,   8,    1,    0,    0,    0,    0,   0,   0,  0},
            {1, 9,  36,  84,  126,  126,  84,   36,   9,    1,    0,    0,    0,   0,   0,  0},
            {1, 10, 45,  120, 210,  252,  210,  120,  45,   10,   1,    0,    0,   0,   0,  0},
            {1, 11, 55,  165, 330,  462,  462,  330,  165,  55,   11,   1,    0,   0,   0,  0},
            {1, 12, 66,  220, 495,  792,  924,  792,  495,  220,  66,   12,   1,   0,   0,  0},
            {1, 13, 78,  268, 715,  1287, 1716, 1716, 1287, 715,  286,  78,   13,  1,   0,  0},
            {1, 14, 91,  364, 1001, 2002, 3003, 3432, 3003, 2002, 1001, 364,  91,  14,  1,  0},
            {1, 15, 105, 455, 1365, 3003, 5005, 6435, 6435, 5005, 3003, 1365, 455, 105, 15, 1},
    };

    std::vector<float> ui;
    std::vector<float> uni;
    std::vector<float> vi;
    std::vector<float> vni;

    static vec3 Casteljau(const std::vector<Point> &pts, const float &u);

    float Bu(const unsigned &i, const unsigned &n);

    float Bv(const unsigned &i, const unsigned &n);

    void initU(const float &u);

    void initV(const float &v);

public:
    explicit BezierSurface(std::vector<std::vector<Point>> ctrl);

    void changeCtrlPts(const std::vector<std::vector<Point>> &newPts);

    vec3 Analytical2D(const float &u, const float &v);

    vec3 Casteljau2D(const float &u, const float &v) const;

    void CalculateSurfacePointsCasteljau(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                         const float &stepV) const;

    void CalculateSurfacePointsAnalytical(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                          const float &stepV);

    void OldCalculateSurfacePointsAnalytical(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                             const float &stepV);

    static Mesh genMesh(std::vector<std::vector<vec3>> &surfacePts);

    void getBounds(Point &minPt, Point &maxPt) const;
};