#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_
#include <ostream>
#include <Eigen/Dense>

template<class T>
struct Vec2
{
    union
    {
        struct { T x, y; };
        struct { T u, v; };
        T raw[2];
    };

    Vec2(T _x, T _y) : x(_x), y(_y) {}

    Vec2() : x(0), y(0) {}

    Vec2<T> operator+(const Vec2<T>& rhl) const { return Vec2<T>(x + rhl.x, y + rhl.y); }
    Vec2<T> operator-() const { return Vec2<T>(-x, -y); }
    Vec2<T> operator-(const Vec2<T>& rhs) const { return *this + (-rhs); }
    Vec2<T> operator*(const float s) const { return Vec2<T>(x * s, y * s); }
    T dot(const Vec2<T>& rhl) const { return x * rhl.x + y * rhl.y; }

    Vec2<T> norm() const { return (*this) * (1.0 / magnitude()); }
    float magnitude() const { return sqrt(x * x + y * y); }

    template<class>
    friend std::ostream& operator<<(std::ostream& os, Vec2<T>& v);
};

template<class T>
struct Vec3
{
    union
    {
        struct { T x, y, z; };
        struct { T u, v, w; };
        struct { T r, g, b; };
        T raw[3];
    };

    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    Vec3() : x(0), y(0), z(0) {}

    Vec3<T> operator+(const Vec3<T>& rhl) const { return Vec3<T>(x + rhl.x, y + rhl.y, z + rhl.z); }
    Vec3<T> operator-() const { return Vec3<T>(-x, -y, -z); }
    Vec3<T> operator*(const float s) const { return Vec3<T>(x * s, y * s, z * s); }
    T dot(const Vec3<T>& rhl) const { return x * x + y * y + z * z; };

    Vec3<T> cross(const Vec3<T>& rhl) const
    {
        return Vec3<T>(y * rhl.z - z * rhl.y, z * rhl.x - x * rhl.z, x * rhl.y - y * rhl.x);
    }

    Vec3<T> norm() const { return (*this) * (1.0 / magnitude()); }
    float magnitude() const { return sqrt(x * x + y * y + z * z); }

    template<class>
    friend std::ostream& operator<<(std::ostream& os, Vec3<T>& v);
};

struct Color3B
{
    union
    {
        struct { uint8_t b, g, r; };
        uint8_t raw[3]{};
    };

    Color3B(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}

    Color3B() : r(0), g(0), b(0) {}

    Color3B& operator=(const uint8_t val)
    {
        r = val;
        g = val;
        b = val;
        return *this;
    }

    Color3B operator*(const double factor)
    {
        return Color3B(
            static_cast<uint8_t>(r * factor),
            static_cast<uint8_t>(g * factor),
            static_cast<uint8_t>(b * factor)
        );
    }

    Color3B operator+(const Color3B& rhs)
    {
        return Color3B(r + rhs.r, g + rhs.g, b + rhs.b);
    }
};

struct Color4B
{
    union
    {
        struct { uint8_t a, b, g, r; };
        struct { Color3B rgb; };
        uint8_t raw[4]{};
    };

    Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 0xff) : r(_r), g(_g), b(_b), a(_a) {}

    Color4B() : r(0), g(0), b(0), a(0xff) {}

    Color4B& operator=(const uint8_t val)
    {
        r = val;
        g = val;
        b = val;
        a = val;
        return *this;
    }

    Color4B& operator=(const Color3B& rhs)
    {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        return *this;
    }

    Color4B operator*(const double factor)
    {
        return Color4B(
            static_cast<uint8_t>(r * factor),
            static_cast<uint8_t>(g * factor),
            static_cast<uint8_t>(b * factor),
            static_cast<uint8_t>(a * factor)
        );
    }

    Color4B operator+(const Color4B& rhs)
    {
        return Color4B(
            r + rhs.r,
            g + rhs.g,
            b + rhs.b,
            a + rhs.a
        );
    }
};

template<typename T>
struct Vertex
{
    T x, y, z;
    Color4B c;

    Vertex(T _x, T _y, T _z, uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : x(_x), y(_y), z(_z), c(_r, _g, _b, _a) {}

    Vertex(T _x, T _y, T _z) : x(_x), y(_y), z(_z), c(0, 0, 0, 0xff) {}

    Vertex(T _x, T _y, T _z, Color4B _c) : x(_x), y(_y), z(_z), c(_c) {}

    Vertex() : x(0), y(0), z(0), c(0, 0, 0, 0xff) {}

    friend Vertex<T> operator*(const Eigen::Matrix4d& mat, const Vertex<T>& v)
    {
        auto v1 = Eigen::Vector4d(v.x, v.y, v.z, 1);
        auto ret = mat * v1;
        return Vertex(ret(0), ret(1), ret(2), v.c);
    }
};
typedef Vertex<double> Vertexd;
typedef Vertex<int> Vertexi;

struct Primitive
{
 public:

};

struct Triangle : public Primitive
{
    Vertexd points[3];
    Vec3<double> normal;

    Triangle(Vertexd a, Vertexd b, Vertexd c)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;
        const Vec3<double> ca = Vec3<double>(a.x - c.x, a.y - c.y, a.z - c.z);
        const Vec3<double> ab = Vec3<double>(b.x - a.x, b.y - a.y, b.z - a.z);
        normal = ca.cross(ab).norm();
    }
};

struct Size
{
    double width, height;
    Size(double w, double h) : width(w), height(h) {}
    Size() = default;
};

struct BoundingBox
{
    Vec2<double> lb_point, rt_point;
    Vec2<double> get_center() const { return (lb_point + rt_point) * 0.5; }
    Size get_size() const
    {
        auto size = rt_point - lb_point;
        return Size(size.x, size.y);
    }
    BoundingBox() = default;
    BoundingBox(Vec2<double> lb, Vec2<double> rt) : lb_point(lb), rt_point(rt) {}
    BoundingBox(double lb_x, double lb_y, double rt_x, double rt_y) : lb_point(Vec2<double>(lb_x, lb_y)),
                                                                      rt_point(Vec2<double>(rt_x, rt_y)) {}

};

template<class T>
std::ostream& operator<<(std::ostream& os, Vec2<T>& v)
{
    os << "(" << v.x << ", " << v.y << ")" << std::endl;
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, Vec3<T>& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    return os;
}

#endif
