#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <cmath>

struct Vec3f
{
	float x, y, z;

    Vec3f() : x(0), y(0), z(0) {}
    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3f operator- (const Vec3f& v) {
        return Vec3f(x - v.x, y - v.y, z - v.z);
    }
    Vec3f operator+ (const Vec3f& v) {
        return Vec3f(x + v.x, y + v.y, z + v.z);
    }
    void  operator+= (const Vec3f& v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    Vec3f operator* (const float& scale) {
        return Vec3f(scale * x, scale * y, scale * z);
    }
    Vec3f operator* (const double& scale) {
        return Vec3f(scale * x, scale * y, scale * z);
    }
    Vec3f cross (Vec3f v) {
        float xNew = v.z*y - v.y*z;
        float yNew = -1 * (v.z*x - v.x*z);
        float zNew = v.y*x - v.x*y;

        return Vec3f(xNew, yNew, zNew);
    }
    double dot(Vec3f v) {
        return x*v.x + y*v.y + z*v.z;
    }
    double magnitude() {
        return sqrt(x*x + y*y + z*z);
    }
    void normalize() {
        float mag = this->magnitude();
        mag = (mag == 0.0 ? 1.0 : mag);
        x /= mag;
        y /= mag;
        z /= mag;
    }
};

struct Vertex
{
    float x, y, z;

    Vertex() : x(0), y(0), z(0) {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Face
{
    int idx1, idx2, idx3;

    Face() : idx1(0), idx2(0), idx3(0) {}
    Face(int i1, int i2, int i3) : idx1(i1), idx2(i2), idx3(i3) {}
};

struct Mesh_Data
{
    std::vector<Vertex*> *vertices;
    std::vector<Face*> *faces;

    Mesh_Data() : vertices(NULL), faces(NULL) {}
    ~Mesh_Data() {}
};



// Computes the area of the triangle bound by the three vertices
static double computeTriangleArea(Vec3f v1, Vec3f v2, Vec3f v3) {
    // Using heron's formula, where triangle with sidelengths a, b, and c
    // and semiperimeter s = (a + b + c) / 2 has area sqrt(s(s-a)(s-b)(s-c))
    float a, b, c, s;

    a = (v3-v2).magnitude();
    b = (v2-v1).magnitude();
    c = (v1-v3).magnitude();
    s = (a + b + c) / 2;

    return sqrt(s * (s-a) * (s-b) * (s-c));
}

#endif
