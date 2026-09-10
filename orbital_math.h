#include "vector3.h"

#define SOLAR_DAY                       360.98564736629

const double gec            = 1.082626 * std::pow(10, -3); //geopotential coefficient
const double eqr            = 6378.137;                    //Earth's equatorial radius (this is KILOMETERS) 6378.137km not meters
const double gmst_j2000     = 280.46061837;
const double r_rate         = 7.292115 * std::pow(10,-5);  //Earth's rotation rate relative to the stars (sidereal)
const double light_speed    = 299792.458l;                  //Speed of light (tnhis is in KILOMETERS per second)


//e2≈0.00669438


struct vector6 
{
    vector3 p;  //positional vector
    vector3 v;  //velocity vector
};