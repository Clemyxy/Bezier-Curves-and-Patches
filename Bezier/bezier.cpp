#include "bezier.hpp"

/**
 *
 * @param i
 * @param n
 * @return
 */
float BezierCurve::B(const unsigned &i, const unsigned &n) {
    return cnp[n][i] * ui[i] * uni[n - i];
}

/**
 *
 * @param u
 */
void BezierCurve::initU(const float &u) {
    float u1 = 1 - u;
    for (int k = 1; k < ctrlPts.size(); ++k) {
        ui[k] = ui[k - 1] * u;
        uni[k] = uni[k - 1] * u1;
    }
}

/**
 * Constructor
 * @param ctrl Controls points
 */
BezierCurve::BezierCurve(std::vector<Point> ctrl) : ctrlPts(std::move(ctrl)) {
    ui.resize(16);
    uni.resize(16);
    ui[0] = 1.f;
    uni[0] = 1.f;
}

/**
 * Change controls points
 * @param newPts New controls points
 */
void BezierCurve::changeCtrlPts(const std::vector<Point> &newPts) {
    ctrlPts.clear();
    ctrlPts = newPts;
}

/**
 * Evaluate curve
 * @param u
 * @return
 */
vec3 BezierCurve::Analytical(const float &u) {
    initU(u);
    unsigned n = ctrlPts.size();
    vec3 pt(0, 0, 0);
    for (int i = 0; i < n; ++i) {
        pt = (Point) pt + ctrlPts[i] * B(i, n - 1);
    }
    return pt;
}

/**
 * Evaluate curve
 * @param u
 * @return
 */
vec3 BezierCurve::Casteljau(const float &u) const {
    std::vector<Point> tmp(ctrlPts);
    unsigned long long n = ctrlPts.size();
    float u1 = 1 - u;
    for (unsigned long long i = 1; i < n; ++i) {
        for (unsigned long long j = 0; j < n - i; ++j)
            tmp[j] = tmp[j] * u1 + tmp[j + 1] * u;
    }
    return tmp[0];
}

/**
 * curvePts must be an empty vector, which will be filled with each calculated point using Casteljau's method
 * @param curvePts Curve points
 * @param step
 */
void BezierCurve::CalculateCurvePointsCasteljau(std::vector<vec3> &curvePts, const float &step) const {
    curvePts.clear();
    for (float i = 0.f; i <= 1.f; i += step) {
        curvePts.emplace_back(Casteljau(i));
    }
}

/**
 * curvePts must be an empty vector, which will be filled with each calculated point using the Analytical method
 * @param curvePts Curve points
 * @param step
 */
void BezierCurve::CalculateCurvePointsAnalytical(std::vector<vec3> &curvePts, const float &step) {
    curvePts.clear();
    for (float i = 0.f; i <= 1.f; i += step) {
        curvePts.emplace_back(Analytical(i));
    }
}

/**
 * Filled `minPt` and `maxPt` with bounds of the curve
 * @param minPt
 * @param maxPt
 */
void BezierCurve::getBounds(Point &minPt, Point &maxPt) const {
    for (const Point pt: ctrlPts) {
        minPt = min(minPt, pt);
        maxPt = max(minPt, pt);
    }
}

/**
 * Make a Surface Of Revolution
 * @param curvePts Point of curve
 * @param rotStep Rotation step
 * @return a Surface of Revolution
 */
Mesh BezierCurve::makeSOR(const std::vector<vec3> &curvePts, const float &rotStep) {
    Mesh tmp(GL_TRIANGLES);

    //Add Vertices to the mesh
    std::vector<vec3> tmpCurve(curvePts);
    for (float angle = 0; angle < 360; angle += rotStep) {
        Transform rot = RotationY(angle);
        for (auto &pt: tmpCurve) {
            tmp.vertex((Point) rot(vec4(pt, 1)));
        }
    }

    //Create Triangles
    unsigned nbRota = tmp.vertex_count() / tmpCurve.size();
    unsigned nb = tmpCurve.size();
    for (int j = 0; j < nbRota - 1; ++j) {
        for (int i = 0; i < nb - 1; ++i) {
            tmp.triangle(j * nb + i, (j + 1) * nb + i, j * nb + i + 1);
            tmp.triangle((j + 1) * nb + i, (j + 1) * nb + i + 1, j * nb + i + 1);
        }
    }
    //Special case (k => 0)
    for (int i = 0; i < nb - 1; ++i) {
        tmp.triangle((nbRota - 1) * nb + i, 0 * nb + i, (nbRota - 1) * nb + i + 1);
        tmp.triangle(0 * nb + i, 0 * nb + i + 1, (nbRota - 1) * nb + i + 1);
    }
    return tmp;
}