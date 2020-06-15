#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_
#include <ostream>
#include <Eigen/Dense>

template<class T>
struct Vec2
{
    union
    {
        struct
        {
            T x, y;
        };

        struct
        {
            T u, v;
        };

        T raw[2];
    };

    Vec2(T _x, T _y) : x(_x), y(_y)
    {
    }

    Vec2() : x(0), y(0)
    {
    }

    Vec2<T> operator+(const Vec2<T>& rhl) const { return Vec2<T>(x + rhl.x, y + rhl.y); }
    Vec2<T> operator-() const { return Vec2<T>(-x, -y); }
    Vec2<T> operator-(const Vec2<T>& rhs) const
    {
        return *this + (-rhs);
    }
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
        struct
        {
            T x, y, z;
        };

        struct
        {
            T u, v, w;
        };

        struct
        {
            T r, g, b;
        };

        T raw[3];
    };

    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z)
    {
    }

    Vec3() : x(0), y(0), z(0)
    {
    }

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
        struct
        {
            uint8_t r, g, b;
        };

        uint8_t raw[3]{};
    };

    Color3B(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b)
    {
    }

    Color3B() : r(0), g(0), b(0)
    {
    }

    Color3B& operator=(const uint8_t val)
    {
        r = val;
        g = val;
        b = val;
        return *this;
    }
};

struct Color4B
{
    union
    {
        struct
        {
            uint8_t r, g, b, a;
        };

        uint8_t raw[4]{};
    };

    Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 0xff) : r(_r), g(_g), b(_b), a(_a)
    {
    }

    Color4B() : r(0), g(0), b(0), a(0xff)
    {
    }

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
};

struct Vertex
{
    float x, y, z;
    Color4B c;

    Vertex(float _x, float _y, float _z, uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : x(_x), y(_y), z(_z), c(_r, _g, _b, _a) {}

    Vertex(float _x, float _y, float _z) : x(_x), y(_y), z(_z), c(0, 0, 0, 0xff) {}

    Vertex(float _x, float _y, float _z, Color4B _c) : x(_x), y(_y), z(_z), c(_c) {}

    Vertex() : x(0), y(0), z(0), c(0, 0, 0, 0xff) {}

    friend Vertex operator*(const Eigen::Matrix4d& mat, const Vertex& v)
    {
        auto v1 = Eigen::Vector4d(v.x, v.y, v.z, 1);
        auto ret = mat * v1;
        return Vertex(ret(0), ret(1), ret(2), v.c);
    }
};

struct Primitive
{
 public:

};

struct Triangle : public Primitive
{
    Vertex points[3];
    Vec3<float> normal;

    Triangle(Vertex a, Vertex b, Vertex c)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;
        const Vec3<float> ca = Vec3<float>(a.x - c.x, a.y - c.y, a.z - c.z);
        const Vec3<float> ab = Vec3<float>(b.x - a.x, b.y - a.y, b.z - a.z);
        normal = ca.cross(ab).norm();
    }
};

struct Size
{
    float width, height;
    Size(float w, float h) : width(w), height(h) {}
    Size() = default;
};

struct BoundingBox
{
    Vec2<float> lb_point, rt_point;
    Vec2<float> get_center() const { return (lb_point + rt_point) * 0.5; }
    Size get_size() const
    {
        auto size = rt_point - lb_point;
        return Size(size.x, size.y);
    }
    BoundingBox() = default;
    BoundingBox(Vec2<float> lb, Vec2<float> rt) : lb_point(lb), rt_point(rt) {}
    BoundingBox(float lb_x, float lb_y, float rt_x, float rt_y) : lb_point(Vec2<float>(lb_x, lb_y)),
                                                                  rt_point(Vec2<float>(rt_x, rt_y)) {}

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
