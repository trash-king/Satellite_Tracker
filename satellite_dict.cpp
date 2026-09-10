#include <iostream>
#include <ostream>
#include <istream>
#include "satellite_dict.h"
#include "nlohmann.h"

SatelliteDictionary::SatelliteDictionary()
{
    generateSatCat();
}

void SatelliteDictionary::readEntry(int entry)
{
    //we can use binary search as the SATIDs are sorted
    int pivot = 0;
    int left = 0;
    int right = user_satcat.size() - 1;
    while(left <= right)
    {
        pivot = left + (right - left) / 2;
        if(user_satcat[pivot].satellite_id == entry)
        {
            std::cout << "Your text here" << std::endl;
        }
        else if(user_satcat[pivot].satellite_id > entry) right = pivot - 1;
        else left = pivot + 1;
    } 
        std::cout << "ERROR" << std::endl;
    return;  
}

Entry SatelliteDictionary::accessEntry(int entry)
{
    //we can use binary search as the SATIDs are sorted
    int pivot = 0;
    int left = 0;
    int right = user_satcat.size() - 1;
    while(left <= right)
    {
        pivot = left + (right - left) / 2;
        if(user_satcat[pivot].satellite_id == entry) return user_satcat[entry];
        else if(user_satcat[pivot].satellite_id > entry) right = pivot - 1;
        else left = pivot + 1;
    } 
    return user_satcat[left];
}

void SatelliteDictionary::appendToUserDict(json * entry)
{

    entry["satid"] =        user_satcat[0].satellite_id;
    entry["satname"] =      user_satcat[0].satellite_name;
    entry["satlatitude"] =  user_satcat[0].latitude;
    entry["satlongitude"] = user_satcat[0].longitude;
    entry["azimuth"] =      user_satcat[0].azimuth;
    entry["elevation"] =    user_satcat[0].elevation;
    entry["ra"] =           user_satcat[0].ra;
    entry["dec"] =          user_satcat[0].declination;
    entry["timestamp"] =    user_satcat[0].timestamp;
}

void SatelliteDictionary::generateSatCat()
{
    std::ifstream f("satcat.json");

}