#include "surface2D.hpp"

/**
 *
 * @param pts
 * @param u
 * @return
 */
vec3 BezierSurface::Casteljau(const std::vector<Point> &pts, const float &u) {
    std::vector<Point> tmp(pts);
    unsigned long long n = tmp.size();
    float u1 = 1 - u;
    for (unsigned long long i = 1; i < n; ++i) {
        for (unsigned long long j = 0; j < n - i; ++j)
            tmp[j] = tmp[j] * u1 + tmp[j + 1] * u;
    }
    return tmp[0];
}

/**
 *
 * @param i
 * @param n
 * @return
 */
float BezierSurface::Bu(const unsigned &i, const unsigned &n) {
    return cnp[n][i] * ui[i] * uni[n - i];
}

/**
 *
 * @param i
 * @param n
 * @return
 */
float BezierSurface::Bv(const unsigned &i, const unsigned &n) {
    return cnp[n][i] * vi[i] * vni[n - i];
}

/**
 *
 * @param u
 */
void BezierSurface::initU(const float &u) {
    float u1 = 1 - u;
    for (int k = 1; k < ctrlPts.size(); ++k) {
        ui[k] = ui[k - 1] * u;
        uni[k] = uni[k - 1] * u1;
    }
}

/**
 *
 * @param v
 */
void BezierSurface::initV(const float &v) {
    float v1 = 1 - v;
    for (int k = 1; k < ctrlPts[0].size(); ++k) {
        vi[k] = vi[k - 1] * v;
        vni[k] = vni[k - 1] * v1;
    }
}

/**
 * Constructor
 * @param ctrl Controls points
 */
BezierSurface::BezierSurface(std::vector<std::vector<Point>> ctrl) : ctrlPts(std::move(ctrl)) {
    ui.resize(16);
    uni.resize(16);
    vi.resize(16);
    vni.resize(16);
    ui[0] = 1.f;
    uni[0] = 1.f;
    vi[0] = 1.f;
    vni[0] = 1.f;
}

/**
 * Change controls points
 * @param newPts New constrols points
 */
void BezierSurface::changeCtrlPts(const std::vector<std::vector<Point>> &newPts) {
    ctrlPts.clear();
    ctrlPts = newPts;
}

/**
 * Compute one point, not the most optimal code for a full surface calculation
 * @param u
 * @param v
 * @return
 */
vec3 BezierSurface::Analytical2D(const float &u, const float &v) {
    initU(u);
    initV(v);
    unsigned n = ctrlPts.size();
    vec3 pt(0, 0, 0);
    for (int i = 0; i < n; ++i) {
        unsigned m = ctrlPts[i].size();
        for (int j = 0; j < m; ++j)
            pt = (Point) pt + ctrlPts[i][j] * Bu(i, n - 1) * Bv(j, m - 1);
    }

    return pt;
}

/**
 * Compute one point of the surface
 * @param u
 * @param v
 * @return
 */
vec3 BezierSurface::Casteljau2D(const float &u, const float &v) const {
    std::vector<Point> pts;
    vec3 pt(0, 0, 0);
    for (const std::vector<Point> &row: ctrlPts)
        pts.emplace_back(Casteljau(row, v));
    return Casteljau(pts, u);
}

/**
 * Compute surface
 * curvePts must be an empty vector, which will be filled with each calculated point
 * @param surfacePts
 * @param stepU
 * @param stepV
 */
void BezierSurface::CalculateSurfacePointsCasteljau(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                                    const float &stepV) const {
    surfacePts.clear();
    for (float i = 0; i <= 1; i += stepU) {
        surfacePts.emplace_back();
        for (float j = 0; j <= 1; j += stepV) {
            surfacePts.back().emplace_back(Casteljau2D(i, j));
        }
    }
}

/**
 * Compute surface
 * @param surfacePts
 * @param stepU
 * @param stepV
 */
void BezierSurface::CalculateSurfacePointsAnalytical(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                                     const float &stepV) {
    surfacePts.clear();
    unsigned nu = ctrlPts.size();
    for (float i = 0; i <= 1; i += stepU) {
        surfacePts.emplace_back();
        initU(i);
        for (float j = 0; j <= 1; j += stepV) {
            initV(j);
            vec3 pt(0, 0, 0);
            for (int k = 0; k < nu; ++k) {
                unsigned nv = ctrlPts[k].size();
                for (int h = 0; h < nv; ++h) {
                    pt = (Point) pt + ctrlPts[k][h] * Bu(k, nu - 1) * Bv(h, nv - 1);
                }
            }
            surfacePts.back().emplace_back(pt);
        }
    }
}

/**
 * Legacy function to calculate the surface using the analytical method, slighly slower than the new
 * @param surfacePts
 * @param stepU
 * @param stepV
 */
void BezierSurface::OldCalculateSurfacePointsAnalytical(std::vector<std::vector<vec3>> &surfacePts, const float &stepU,
                                                        const float &stepV) {
    surfacePts.clear();
    for (float i = 0; i <= 1; i += stepU) {
        surfacePts.emplace_back();
        for (float j = 0; j <= 1; j += stepV) {
            surfacePts.back().emplace_back(Analytical2D(i, j));
        }
    }
}

/**
 * Make a surface
 * @param surfacePts Points of surface
 * @return a surface
 */
Mesh BezierSurface::genMesh(std::vector<std::vector<vec3>> &surfacePts) {
    Mesh tmp(GL_TRIANGLES);
    unsigned n = surfacePts.size();
    for (int i = 0; i < n; ++i)
        for (auto &pt: surfacePts[i])
            tmp.vertex(pt);
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < surfacePts[i].size() - 1; ++j) {
            tmp.triangle(i * n + j, i * n + j + 1, (i + 1) * n + j);
            tmp.triangle((i + 1) * n + j, i * n + j + 1, (i + 1) * n + j + 1);
        }
    return tmp;
}

/**
 * Get the bounding Box of the surface
 * @param minPt
 * @param maxPt
 */
void BezierSurface::getBounds(Point &minPt, Point &maxPt) const {
    for (const std::vector<Point> &row: ctrlPts) {
        for (const Point pt: row) {
            minPt = min(minPt, pt);
            maxPt = max(minPt, pt);
        }
    }
}