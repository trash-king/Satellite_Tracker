#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include "vector3.h"
#include "orbital_math.h"
#define ONLY_C_LOCALE   1       //fixes a bug in the date/date.h library      
#include "date/date.h"

using namespace std::chrono;

//GMST - Greenwich Mean Sidereal Time
//j2000 can be delineated by either seconds or days since January 1st, 2000.

double time_since_j2000(double unix_seconds)
{
    auto j2000 = date::sys_days{date::year{2000}/1/1} + std::chrono::hours{12};
    auto cur = std::chrono::system_clock::now();

    double j2000_unix = std::chrono::duration_cast<std::chrono::seconds>(j2000.time_since_epoch()).count();
    return (unix_seconds - j2000_unix) / 86400;
}

double gmst(double unix_seconds) //j2000 - Reference epoch is January 1st, 2000. J2000 is time since this reference point  //jd Julian Date - number of solar days since J2000 
{
    //280.46061837
   double ref_epoch = time_since_j2000(unix_seconds);
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

/*
ECI vector
Z-axis points to the North Pole
X-axis points towards the vernal equinox
Y-axis completes a right-handed system in the equitorial plane (is always 90 degrees perpendicular to the X value)

ECEF vector
Z-axis points to the North Pole
X-axis always points to the equator but moves with the Earth
Y-axis completes a right-handed system (is always 90 degrees perpendicular to the X value)

Shorthand- if the X and Y equations were plotted, Y would always be exactly 90 degrees to X

*/

vector3 generate_ECI_vector(vector3 ecef_vector, double unix_timestamp)                        //earth-centered initial (takes a time value)
{
    vector3 eci_vector;
    double g_time = gmst(unix_timestamp) * (M_PI/180);
    eci_vector.x = (ecef_vector.x * cos(g_time) - ecef_vector.y * sin(g_time)); //X
    eci_vector.y = (ecef_vector.x * sin(g_time) + ecef_vector.y * cos(g_time)); //Y
    eci_vector.z = (ecef_vector.z);                                             //Z

    return eci_vector;
}

vector3 generate_ECI_vector_at_point(vector3 ecef_vector, double gmst_degrees)              //earth-centered initial at specified timestamp (takes the angle between two X axes)
{
    vector3 eci_vector;
    double g_time = gmst_degrees * (M_PI/180);
    eci_vector.x = (ecef_vector.x * cos(g_time) - ecef_vector.y * sin(g_time)); //X
    eci_vector.y = (ecef_vector.x * sin(g_time) + ecef_vector.y * cos(g_time)); //Y
    eci_vector.z = (ecef_vector.z);                                             //Z

    return eci_vector;   
}

vector3 get_ECI_velocity(vector3 snapshot1, vector3 snapshot2, double v1, double v2)
{
    double t1 = gmst(v1);
    double t2 = gmst(v2);

    vector3 s1 = generate_ECEF_vector(snapshot1.x, snapshot1.y, snapshot1.z);
    vector3 s2 = generate_ECEF_vector(snapshot2.x, snapshot2.y, snapshot2.z);

    s1 = generate_ECI_vector_at_point(s1, t1);
    s2 = generate_ECI_vector_at_point(s2, t2);

    return scalar_division((s2 - s1), (v2 - v1));
}
//True Equator, Mean Equinox
vector3 teme_to_ecef(vector3 teme_vector, double unix_timestamp)
{
    vector3 ecef_vector;
    double g_time = gmst(unix_timestamp) * (M_PI/180);
    ecef_vector.x = ((teme_vector.x * cos(g_time)) + (teme_vector.y * sin(g_time)));
    ecef_vector.y = ((-1 * (teme_vector.x * sin(g_time))) + (teme_vector.y * cos(g_time)) );
    ecef_vector.z = teme_vector.z;

    return ecef_vector;
}

vector3 teme_to_ecef_at_point(vector3 teme_vector, double gmst_degrees)
{
    vector3 ecef_vector;
    double g_time = gmst_degrees * (M_PI/180);
    ecef_vector.x = teme_vector.x * cos(g_time) + teme_vector.y * sin(g_time); //X
    ecef_vector.y = -teme_vector.x * sin(g_time) + teme_vector.y * cos(g_time); //Y
    ecef_vector.z = (teme_vector.z);                                             //Z

    return ecef_vector;  
}

vector3 compute_acceleration(vector3 rECI)
{
    double mag = magnitude(rECI);
    double factor = -mu / (mag * mag * mag);


    return vector3{factor * rECI.x, factor * rECI.y,factor * rECI.z};
}


//returns the rate of change at position 
vector6 derivative(vector6 state)
{
    return vector6{state.v, compute_acceleration(state.p)};
}

vector6 runge_kutta(vector6 d, double step)
{
    vector6 k1 = derivative(d);
    vector6 k2 = derivative(d + k1 * (step/2.0));
    vector6 k3 = derivative(d + k2 * (step/2.0));
    vector6 k4 = derivative(d + k3 * step);

    return d + (k1 + k2*2.0 + k3*2.0 + k4) * (step/6.0);
}

std::vector<vector6> calculate_trajectory(vector6 vec, double duration)
{
    std::vector<vector6> current_trajectory;
    vector6 state = vec;
    current_trajectory.push_back(state);

    int steps = static_cast<int>(duration/step);

    for(int i = 0; i < steps; ++i)
    {
        state = runge_kutta(state, step);
        current_trajectory.push_back(state);
        
    }

    return current_trajectory;

}
