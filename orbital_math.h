#include "vector3.h"

#define SOLAR_DAY                       360.98564736629

const double gec        = 1.082626 * std::pow(10, -3); //geopotential coefficient
const double eqr        = 6378.137;                    //Earth's equatorial radius (this is KILOMETERS) 6378.137km not meters
const double gmst_j2000 = 280.46061837;
//e2≈0.00669438


struct vector6 
{
    vector3 p;  //positional vector
    vector3 v;  //velocity vector
};