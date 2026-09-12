#include <iostream>
#include <ostream>
#include <istream>

#include <curl/curl.h>
#include "satellite_dict.h"

SatelliteDictionary::SatelliteDictionary(int ins)
{
    this->instance = ins;
    std::cout << "constructor called" << std::endl;
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

static size_t SatelliteDictionary::write_callback(char * ptr, size_t size, size_t new_member, void* user_data)
{
    auto output = static_cast<std::string*>(user_data);
    output->append(ptr, size * new_member);
    return size  * new_member;
}

json SatelliteDictionary::getDataFromURL()
{
    CURL* curl = curl_easy_init();
    std::string body;
    long status_code = 0;
   // return "https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (satellite_id, api_key);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK || status_code != 200) return "";
    return json::parse(body);
}

void SatelliteDictionary::getSatelliteData(std::string satellite_id)
{
    json outputData = getDataFromURL();
    vector<json> all_items;
    for(auto & [key, value] : outputData.items())
    {
        all_items.push_back(value);
    }
    for(int i = 0; i < all_items.size() - 1; i++)
    {
        std::cout << all_items[i] << std::endl;
    }
    
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