#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include <vector>
#include "vector3.h"
#include "orbital_math.h"
#include "date/date.h"

using namespace std::chrono;

/*

For a satellite (negligible mass) orbiting Earth (treated as a point mass), in an Earth-Centered Inertial frame:

𝑟
⃗
¨
=
−
𝜇
𝑟
3
𝑟
⃗
r
¨
=−
r
3
μ
	​

r

r = (x,y,z) - position relative to Earth's center
r = |r| - absolute value of r
μ = standard gravitational parameter (3.968 x 10^14)

v subscript x = sgp * x /r cubed
v subscript x = sgp * x /r cubed

mu is the standard gravitational parameter of Earth

x˙=vx​,y˙​=vy​,z˙=vz
derivative of x of t (time) = vx
derivative of y of t (time) = vy
derivateive of z of t (time) = vz

(\(\dot{v}_x, \dot{v}_y, \dot{v}_z\)) it's velocity vector over time

Core Equation-
the second derivative of r(t) (position relative to Earth's center over time)
is equivalent to
(standard gravitation parameter / (x,y,z)^3) multiplied by (x,y,z)

to find r, use the 3D pythagorean theorem sqrt(x^2 + y^2 + z^2)
(x, y, z) is the satellite's 3D position relative to the center of Earth
if Earth's center is 0,0,0, the satellite is at x, y, z

How far is the satellite from Earth?
N(o) = a / sqrt(1 - e^2 sin^2 o)
function of N to latitude

convert latitude to radians.
sin of (o)
square sin of (o)

X = (N+h)cos o cos lambda
Y = (N+h)cos o sin lambda
Z = (N(1-e^2)+h)sin o

a=
e^2 = World Geodetic System 1984 = 0.00669438
o = latitude
lambda = longitude
h = altitude

x = X cos 

*/

/*
NORAD ID:    
25544
LOCAL TIME:    
18:28:11
UTC:    
22:28:11
LATITUDE:    
-50.06
LONGITUDE:    
-119.86
ALTITUDE [km]:    
440.19
ALTITUDE [mi]:    
273.52
SPEED [km/s]:    
7.65
SPEED [mi/s]:    
4.75
AZIMUTH:    178.0 S  178 degrees based on true north
ELEVATION:    
-46.1
RIGHT ASCENSION:    
20h 21m 35s
DECLINATION:    
-89° 36' 49''

where 
𝑎
a=6378.137 km, 
𝑒
2
e
2
≈0.00669438 (WGS84), 
𝜙
ϕ=latitude, 
𝜆
λ=longitude, 
ℎ
h=altitude.

*/

//float r = vector3(1,2,3);

//GMST - Greenwich Mean Sidereal Time
//j2000 can be delineated by either seconds or days since January 1st, 2000.

double time_since_j2000()
{
    auto j2000 = date::sys_days{date::year{2000}/1/1};
    auto cur = std::chrono::system_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::duration<double, date::days::period>>(cur-j2000);
    return duration.count();
}

double gmst() //j2000 - Reference epoch is January 1st, 2000. J2000 is time since this reference point  //jd Julian Date - number of solar days since J2000 
{
    //280.46061837
   double ref_epoch = time_since_j2000();
   return std::fmod(gmst_j2000 + SOLAR_DAY * (ref_epoch), 360.0); //
}

double radius_of_curvature(double latitude)
{   
    double rad =    latitude * (M_PI/180);
    double sin_2 =  pow(std::sin(rad),2);
    double denom = std::sqrt(1 - (0.081819 * 0.081819) * sin_2);
    return eqr / denom;
}

vector3 generate_ECEF_vector(double longitude, double latitude, double altitude)    //earth-centered earth-fixed
{
    //X=(N+h)cosϕcosλ
    //Y=(N+h)cosϕsinλ
    //(N(1−e2)+h)sinϕ
    
    double c_long = longitude * (M_PI/180);
    double c_lat  = latitude  * (M_PI/180);
    vector3 ecef_vec;

    double n = radius_of_curvature(latitude);
    
    ecef_vec.x = (n + altitude)* cos(c_lat)                      *  cos(c_long);
    ecef_vec.y = (n + altitude)* cos(c_lat)                      *  sin(c_long);
    ecef_vec.z = (n *(1 - (0.081819 * 0.081819))+ altitude)      *  sin(c_lat);
    
    return ecef_vec;
}



vector3 generate_ECI_vector(vector3 ecef_vector)                                //earth-centered initial
{
    vector3 eci_vector;
    double g_time = gmst() * (M_PI/180);
    eci_vector.x = (ecef_vector.x * cos(g_time) - ecef_vector.y * sin(g_time)); //X
    eci_vector.y = (ecef_vector.x * sin(g_time) + ecef_vector.y * cos(g_time)); //Y
    eci_vector.z = (ecef_vector.z);                                             //Z

    return eci_vector;
}

vector3 generate_ECI_vector(vector3 ecef_vector, double timestamp)              //earth-centered initial at specified timestamp
{
    vector3 eci_vector;
    double g_time = timestamp * (M_PI/180);
    eci_vector.x = (ecef_vector.x * cos(g_time) - ecef_vector.y * sin(g_time)); //X
    eci_vector.y = (ecef_vector.x * sin(g_time) + ecef_vector.y * cos(g_time)); //Y
    eci_vector.z = (ecef_vector.z);                                             //Z

    return eci_vector;   
}

vector3 get_ECI_velocity(vector3 snapshot1, vector3 snapshot2, double t1, double t2)
{

    vector3 s1 = generate_ECEF_vector(snapshot1.x, snapshot1.y, snapshot1.z);
    vector3 s2 = generate_ECEF_vector(snapshot2.x, snapshot2.y, snapshot2.z);

    s1 = generate_ECI_vector(s1, t1);
    s2 = generate_ECI_vector(s2, t2);

    return scalar_division((s2 - s1), (t2 - t1));
}

vector3 compute_acceleration(vector3 rECI)
{
    double mag = magnitude(rECI);
    double factor = -mu / (mag * mag * mag);
    return vector3{factor * rECI.x, factor * rECI.y,factor * rECI.z};
}

vector6 derivative_of_position(vector6 state)
{
    return vector6{state.v, compute_acceleration(state.p)};
}

vector6 runge_kutta(vector6 d, double h)
{
    vector6 k1 = derivative_of_position(d);
    vector6 k2 = derivative_of_position(d + k1 * (h/2.0));
    vector6 k3 = derivative_of_position(d + k2 * (h/2.0));
    vector6 k4 = derivative_of_position(d + k3 * h);

    return (d + (h/6)) * (k1 + (k2 * 2.0) + (k3 * 2.0) + k4);
}

void calculate_trajectory(vector6 vec)
{
    std::vector<vector6> current_trajectory;
    current_trajectory.push_back(vec);

    for(int i = 0; i < 60; i += step)
    {
        current_trajectory.push_back(runge_kutta(vec, step));
    }

}

//rECI is the result of generate_ECI_vector

/*
how to calculate rECEF: rECEF is the velocity vector of Earth-Centered Earth-Fixed
rotation. This is the velocity of the rotation of ECEF. Earth-Centered Earth Fixed vector rotates at a velocity ofX

vECEF = r*ECEF(t2) - r*ECEF(t1)/t2- t1
What this means is to take the rotational vector of ECEF at two specific points in time, then find the
rate between the two points.
Or we can pull it from Celestrack or some other source. THis is the ECEF of Earth, it doesn't change

*/

//ECI is non-rotating, but ECEF rotates with Earth
//ECI is fixed, while ECEF is not fixed

//vECI - ECI velocity vector. This is a velocity vector relative to Earth's rotation rate
//vECI = R(θ) (R theta is A rotation matrix, vector R rotated by angle theta. Theta refers to Greenwich Mean Sidereal Time)
//R(theta) = generate_ECI_vector

//vECI (velocity vector) = generate_ECI_vector * vECEF + Earth's rotation rate * rECI


/*
X = (N+h)cos o cos lambda
Y = (N+h)cos o sin lambda
Z = (N(1-e^2)+h)sin o

ωE ≈7.292115×10^-5 rad/s, Earth's rotation rate. Earth rotates at a rate of 7.292115x10^-5 radians per second
This is measured relative to the stars (sidereal) as opposed to the sun.
E
	​

Alpha – Α α
Beta – Β β
Gamma – Γ γ
Delta – Δ δ
Epsilon – Ε ε
Zeta – Ζ ζ
Eta – Η η
Theta – Θ θ
Iota – Ι ι
Kappa – Κ κ
Lambda – Λ λ
Mu – Μ μ
Nu – Ν ν
Xi – Ξ ξ
Omicron – Ο ο
Pi – Π π
Rho – Ρ ρ
Sigma – Σ σ 
ς (final sigma)
Tau – Τ τ
Upsilon – Υ υ
Phi – Φ φ
Chi – Χ χ
Psi – Ψ ψ
Omega – Ω ω
*/