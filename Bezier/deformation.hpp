#pragma once

#include <mesh.h>
#include <cmath>
#include <functional>
#include <utility>
#include <iostream>

static bool insideBox(const vec3 &pt, const vec3 &pmin, const vec3 &pmax) {
    return (pt.x >= pmin.x)
           && (pt.y >= pmin.y)
           && (pt.z >= pmin.z)
           && (pt.x <= pmax.x)
           && (pt.y <= pmax.y)
           && (pt.z <= pmax.z);
}

static void GlobalToLocal(Mesh &obj, vec3 &movevec) {
    Point pmin, pmax;
    vec3 center;
    obj.bounds(pmin, pmax);
    center = (pmax - pmin) / 2;
    movevec = pmin + (Point) center;
    for (int i = 0; i < obj.vertex_count(); ++i) {
        obj.vertex(i, (Point) obj.positions()[i] - (Point) movevec);
    }
}

static void LocalToGlobal(Mesh &obj, const vec3 &movevec) {
    for (int i = 0; i < obj.vertex_count(); ++i) {
        obj.vertex(i, (Point) obj.positions()[i] + (Point) movevec);
    }
}

class Taper {
private:
    static float applyFunc(const float &z, const float &coeff, const float &rMin, const float &rMax) {
        if (z < rMin)
            return 1;
        if (z > rMax)
            return coeff;

        auto interval = rMax - rMin;
        auto coeffred = 1 - (z - rMin) / interval;
        return coeff + coeffred * coeff;
    }

    //To use on a point in the local space, not the world space
    static vec3 TaperVec(const vec3 &pt, unsigned axis, const float &coeff, const float &rMin, const float &rMax) {
        assert(axis < 3u);
        float r = applyFunc(pt(axis), coeff, rMin, rMax);

        auto rx = axis == 0u ? 1.f : r;
        auto ry = axis == 1u ? 1.f : r;
        auto rz = axis == 2u ? 1.f : r;
        return vec3(rx * pt.x, ry * pt.y, rz * pt.z);
    }

public:
    static void TaperMesh(Mesh &obj, unsigned axis, const float &coeff, const float &rMin, const float &rMax) {
        vec3 movevec;
        GlobalToLocal(obj, movevec);
        float newMin = rMin - movevec(axis);
        float newMax = rMax - movevec(axis);
        for (int i = 0; i < obj.vertex_count(); ++i) {
            vec3 curr = TaperVec(obj.positions()[i], axis, coeff, newMin, newMax);
            obj.vertex(i, curr);
        }
        LocalToGlobal(obj, movevec);
    }

    static void LocalTaper(Mesh &obj, unsigned axis, const vec3 &pMin, const vec3 &pMax, const float &coeff) {
        vec3 movevec;
        std::vector<unsigned> points;
        for (int i = 0; i < obj.vertex_count(); ++i) {
            if (insideBox(obj.positions()[i], pMin, pMax))
                points.push_back(i);
        }
        GlobalToLocal(obj, movevec);
        Point i, j;
        obj.bounds(i, j);
        float newMin = pMin(axis) - movevec(axis);
        float newMax = pMax(axis) - movevec(axis);
        for (unsigned iPoint: points) {
            vec3 curr = TaperVec(obj.positions()[iPoint], axis, coeff, newMin, newMax);
            obj.vertex(iPoint, curr);
        }
        LocalToGlobal(obj, movevec);
    }
};

static void twist(Mesh &object, unsigned axis, std::function<float(float)> f, std::function<bool(const vec3 &)> p) {
    assert(axis < 3u);
    auto twistPoint = [&](vec3 &point) -> void {
        if (!p(point)) {
            return;
        }

        auto theta = f(point(axis));
        auto C = std::cos(theta);
        auto S = std::sin(theta);

        auto x = point((axis + 1u) % 3u);
        auto y = point((axis + 2u) % 3u);

        point((axis + 1u) % 3u) = x * C - y * S;
        point((axis + 2u) % 3u) = x * S + y * C;
    };
    vec3 movevec;
    GlobalToLocal(object, movevec);
    auto points = object.positions();
    std::for_each(points.begin(), points.end(), twistPoint);
    for (auto i = 0u; i < points.size(); i++) {
        const auto &point = points.at(i);
        object.vertex(i, point);
    }
    LocalToGlobal(object, movevec);
}

static void globalTwist(Mesh &object, unsigned axis, std::function<float(float)> f) {
    twist(object, axis, std::move(f), [](const vec3 &) -> bool { return true; });
}

static void globalTwist(Mesh &object, unsigned axis) {
    globalTwist(object, axis, [](float z) -> float { return z; });
}

static void localTwist(Mesh &object, unsigned axis, vec3 boundMin, vec3 boundMax, std::function<float(float)> f) {
    Point pmin, pmax;
    object.bounds(pmin, pmax);
    auto center = (pmax - pmin) / 2;
    auto movevec = pmin + center;

    boundMin = Point(boundMin) - movevec;
    boundMax = Point(boundMax) - movevec;

    auto p = [&](const vec3 &point) -> bool {
        return insideBox(point, boundMin, boundMax);
    };
    twist(object, axis, std::move(f), p);
}

static void localTwist(Mesh &object, unsigned axis, const vec3 &boundMin, const vec3 &boundMax) {
    localTwist(object, axis, boundMin, boundMax, [](float z) -> float { return z; });
}