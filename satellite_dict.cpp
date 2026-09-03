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

void SatelliteDictionary::appendToUserDict(json * entry)
{
    entry["satid"] = 
    entry["satname"] = 
    entry["satlatitude"] = 
    entry["satlongitude"] = 
    entry["azimuth"] = 
    entry["elevation"] = 
    entry["ra"] = 
    entry["dec"] = 
    entry["timestamp"] = 
}

void SatelliteDictionary::generateSatCat()
{
    std::ifstream f("satcat.json");

}