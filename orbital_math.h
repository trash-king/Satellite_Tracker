#include "vector3.h"

struct vector6 
{
    vector3 p;  //positional vector
    vector3 v;  //velocity vector

    vector6() : p(0,0,0), v(0,0,0) {};
    vector6(double px_, double py_, double pz_, double vx_, double vy_, double vz_) : p(px_,py_,pz_), v(vx_, vy_,vz_) {};


    vector6 operator+(vector6 vec) const {return vector6(p.x + vec.p.x, p.y + vec.p.y, p.z + vec.p.z, v.x + vec.v.x, v.y + vec.v.y, v.z + vec.v.z);}
    vector6 operator+(double w) const {return vector6(p.x + w, p.y + w, p.z + w, v.x + w, v.y + w, v.z + w);}
    vector6 operator*(double w) const {return vector6(p.x * w, p.y * w, p.z * w, v.x * w, v.y * w, v.z * w);}
    vector6 operator*(vector6 vec) const {return vector6(p.x * vec.p.x, p.y * vec.p.y, p.z * vec.p.z, v.x * vec.v.x, v.y * vec.v.y, v.z * vec.v.z);}
};


#define SOLAR_DAY                       360.98564736629

const double gec            = 1.082626 * std::pow(10, -3); //geopotential coefficient
const double eqr            = 6378.137;                    //Earth's equatorial radius (this is KILOMETERS) 6378.137km not meters
const double gmst_j2000     = 280.46061837;
const double r_rate         = 7.292115 * std::pow(10,-5);  //Earth's rotation rate relative to the stars (sidereal)
const double light_speed    = 299792.458l;                  //Speed of light (tnhis is in KILOMETERS per second)
const double mu             = 398600.4418; //kilometers cubed / seconds squared // gravitational parameter of Earth
const double step           = 15.0;         //step size for Runge-Kutta equations.

//e2≈0.00669438

double time_since_j2000();
double gmst();
double radius_of_curvature(double latitude);
vector3 generate_ECEF_vector(double longitude, double latitude, double altitude);
vector3 generate_ECI_vector(vector3 ecef_vector);
vector3 generate_ECI_vector(vector3 ecef_vector, double timestamp);
vector3 get_ECI_velocity(vector3 snapshot1, vector3 snapshot2, double t1, double t2);
vector3 compute_acceleration(vector3 rECI);
vector6 derivative_of_position(vector6 state);
vector6 runge_kutta(vector6 d, double h);
void calculate_trajectory(vector6 vec);
