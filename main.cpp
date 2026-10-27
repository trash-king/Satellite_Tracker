#include <iostream>
#include <ostream>
#include <istream>
#include <unordered_map>

#include "orbital_math.h"
#include "nlohmann/json.hpp"

#include <curl/curl.h>


/*



*/


using namespace nlohmann;

bool is_running  = true;
 
struct Entry
{
    int  entry_id;
    char * satellite_name;
    int satellite_id;
    char * launch_number;
    char * launch_year;
    char * launch_piece;
    char * epoch_day;
    char * epoch_year;
    char * ballistic_coefficient;
    char * radiation_drag;
    char * latitude;
    char * longitude;
    char * azimuth;
    char * elevation;
    char * ra;
    char * declination;
    char * timestamp;

    Entry(int id, char * s_n, int s_i, char * ln, char * ly, char * lp,
          char * ed, char * ey, char * bc, char * rd) :
          entry_id(id), satellite_name(s_n), satellite_id(s_i), launch_number(ln),
          launch_year(ly), launch_piece(lp), epoch_day(ed), epoch_year(ey), ballistic_coefficient(bc),
          radiation_drag(rd) {};

};

std::unordered_map<int, Entry>satellite_dictionary = {};

void to_json(json& j, const Entry& p)
{
    j =json{{"satellite_id", p.satellite_id}, {"satellite_name", p.satellite_name}, {"launch_number", p.launch_number}, {"launch_year", p.launch_year}, {"launch_piece", p.launch_piece}, {"epoch_day", p.epoch_day}, {"epoch_year", p.epoch_year}
    , {"launch_piece", p.launch_piece}, {"epoch_day", p.epoch_day}, {"epoch_year", p.epoch_year}, {"ballistic_coefficient", p.ballistic_coefficient}, {"radiation_drag", p.radiation_drag}, {"latitude", p.latitude}, {"azimuth", p.azimuth}
    , {"elevation", p.elevation}, {"ra", p.ra}, {"declination", p.declination}, {"timestamp", p.timestamp}};
}

void from_json(json& j, const Entry& p)
{
    j.at("satellite_name").get_to(p.satellite_name);
    j.at("launch_number").get_to(p.launch_number);
    j.at("launch_year").get_to(p.launch_year);
    j.at("launch_piece").get_to(p.launch_piece);
    j.at("epoch_day").get_to(p.epoch_day);
    j.at("epoch_year").get_to(p.epoch_year);
    j.at("ballistic_coefficient").get_to(p.ballistic_coefficient);
    j.at("radiation_drag").get_to(p.radiation_drag);
    j.at("satellite_id").get_to(p.satellite_id);
    j.at("latitude").get_to(p.latitude);
    j.at("longitude").get_to(p.longitude);
    j.at("azimuth").get_to(p.azimuth);
    j.at("elevation").get_to(p.elevation);
    j.at("ra").get_to(p.ra);
    j.at("declination").get_to(p.declination);
    j.at("timestamp").get_to(p.timestamp);

}

class SatelliteDictionary
{
    public:
    SatelliteDictionary(int ins);
    void readEntry(int entry);                                    //reads an entry from the dictionary
    Entry accessEntry(int entry);                         //accesses an object from the dictionary
    size_t write_callback(char * ptr, size_t size, size_t new_member, void* user_data);
    json getDataFromURL();
    protected:
    int instance;
    std::vector<Entry> user_satcat;
    json satcat;
    private:
    void generateSatCat();

};

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

size_t SatelliteDictionary::write_callback(char * ptr, size_t size, size_t new_member, void* user_data)
{
    auto output = static_cast<std::string*>(user_data);
    output->append(ptr, size * new_member);
    return size  * new_member;
}

json SatelliteDictionary::getDataFromURL(std::string satellite_id, std::string api_key)
{
    CURL* curl = curl_easy_init();
    std::string body;
    long status_code = 0;
   // return "https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (satellite_id, api_key);
    std::string s_url = "https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s",satellite_id,api_key

    curl_easy_setopt(curl, CURLOPT_URL, s_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK || status_code != 200) return "";
    return json::parse(body);
    //returns a parsed JSON object - we then extract the values from n2yo's json value
}

Entry SatelliteDictionary::accessEntry(int entry)
{++
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

void SatelliteDictionary::generateSatCat()
{
    std::ifstream f("satcat.json");
    
}

SatelliteDictionary * d_tron = new SatelliteDictionary(1);

class SatTracker
{
    public:
    SatTracker();
    void selectSatellite();
    void displayLaunchData();
    void displayEpochData();
    void displayOrbitalData();

    //"https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (tracked_satellite.satellite_id, tracked_satellite.satellite_api_key)
    protected:
    char * getLaunchData();      //returns a data object instead of displaying data     
    char * getEpochData();
    char * getOrbitalData();
    
    const double gec = 1.082626 * std::pow(10, -3); //geopotential coefficient
    const double eqr = 6378.137;                    //Earth's equatorial radius
    private:
    const char * api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";

};

SatTracker::SatTracker()
{
    std::cout << "SatTracker constructor called" << std::endl;
}

void SatTracker::selectSatellite()
{
    std::cout << "selectSatellite() called" << std::endl;
}

SatTracker * s_tron          = new SatTracker();

int main()
{

    return 0;
}