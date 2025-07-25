#ifndef RDP_GEOMETRY_H
#define RDP_GEOMETRY_H

#include <array>
#include <cmath>
#include <algorithm>
#include <initializer_list>

namespace rdp {

struct Vec2D {
    double x = 0.0;
    double y = 0.0;

    Vec2D() = default;
    Vec2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D operator+(const Vec2D& other) const {
        return Vec2D(x + other.x, y + other.y);
    }

    Vec2D operator-(const Vec2D& other) const {
        return Vec2D(x - other.x, y - other.y);
    }

    Vec2D operator*(double s) const {
        return Vec2D(x * s, y * s);
    }

    double dot(const Vec2D& other) const {
        return x * other.x + y * other.y;
    }

    double lengthSquared() const {
        return dot(*this);
    }

    double length() const {
        return std::sqrt(lengthSquared());
    }
};

struct Vec3D {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3D() = default;
    Vec3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3D operator+(const Vec3D& other) const {
        return Vec3D(x + other.x, y + other.y, z + other.z);
    }

    Vec3D operator-(const Vec3D& other) const {
        return Vec3D(x - other.x, y - other.y, z - other.z);
    }

    Vec3D operator*(double s) const {
        return Vec3D(x * s, y * s, z * s);
    }

    double dot(const Vec3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3D cross(const Vec3D& other) const {
        return Vec3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    double lengthSquared() const {
        return dot(*this);
    }

    double length() const {
        return std::sqrt(lengthSquared());
    }
};

struct Point2D {
    double x = 0.0;
    double y = 0.0;

    Point2D() = default;
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D operator-(const Point2D& other) const {
        return Vec2D(x - other.x, y - other.y);
    }

    Point2D operator+(const Vec2D& v) const {
        return Point2D(x + v.x, y + v.y);
    }
};

struct Point3D {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Point3D() = default;
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3D operator-(const Point3D& other) const {
        return Vec3D(x - other.x, y - other.y, z - other.z);
    }

    Point3D operator+(const Vec3D& v) const {
        return Point3D(x + v.x, y + v.y, z + v.z);
    }
};

inline double point2LineDistanceSquared2D(const Point2D &p, const Point2D &a, const Point2D &b) {
    Vec2D ab = b - a;

    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
    double distance = ab.y * (p.x - a.x) - ab.x * (p.y - a.y);
    
    return (distance * distance) / ab.lengthSquared();
}

inline double point2LineDistanceSquared3D(const Point3D &p, const Point3D &a, const Point3D &b) {
    Vec3D ab = b - a;

    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Another_vector_formulation
    return (p - a).cross(ab).lengthSquared() / ab.lengthSquared();
}

} // namespace rdp
#endif // RDP_GEOMETRY_H