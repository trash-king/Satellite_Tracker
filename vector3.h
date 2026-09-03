#include <cmath>

struct vector3{
    double x, y, z;

    vector3() : x(0), y(0),z(0) {};
    vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}


    vector3 operator+(const vector3& o) const {return vector3(x + o.x, y + o.y, z+ o.z);}
    vector3 operator-(const vector3& o) const {return vector3(x - o.x, y - o.y, z- o.z);}
    vector3 operator*(double w) const {return vector3(x * w, y * w, z * w);}

    double dot(const vector3& o) const {return x * o.x + y * o.y + z *  o.z;}
    vector3 cross(const vector3& o) const {
        return vector3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);

    }

    double length() const {return std::sqrt(x * x + y * y + z * z);}
    //quake fast inverse square root
    vector3 normalized() const{
        double len = length();
        if(len < 1e-12) return vector3(0,0,0);
        return vector3(x / len, y / len, z / len);
    }

};