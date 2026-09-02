#include <ostream>
#include <istream>
#include "satellite_dict.h"

SatelliteDictionary::SatelliteDictionary()
{
    generateSatCat();
}

void SatelliteDictionary::readEntry()
{
    
}

json SatelliteDictionary::accessEntry()
{

}

void SatelliteDictionary::generateSatCat()
{
    std::ifstream f("satcat.json");

    


}