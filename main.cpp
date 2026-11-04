#include <iostream>
#include <ostream>
#include <istream>
#include <unordered_map>
#include <fstream>
#include <stdexcept>
#include <cctype>

#include "orbital_math.h"
#include "nlohmann/json.hpp"

#include <curl/curl.h>
#define  TLE_CHECKSUM_IDX         69

using namespace nlohmann;

bool is_running  = true;

struct TLE_Element
{
    TLE_Element(std::strint t);
    bool checksum(std::string x, std::string y);
    double epoch_year;
    double epoch_day;
    double inclination;
    double raan; //Right Ascension of Ascending Node
    double perigree;
    double anomaly;
    double eccentricity;
    double mean_motion;
    double b_drag;
    std::string tle;
    //std::string sub = str.substr(start_index, length)
    //TLE is separated into two lines
    //WE NEED
    /*
        Epoch year      -       19–20 - (line 1)
        Epoch day               21–32 - (line 1)
        Inclination             9–16
        RAAN    18–25
        Argument of perigree    35–42
        Mean anomaly    44–51
        Eccentricity    27–33
        Mean motion     53–63
        B* drag         54–61
    
    */
}

TLE_Element(std::string t)
{
    std::string x = t.substr(0,69);
    std::string y = t.substr(72,141);

    checksum(x, y);

    this->epoch_year   = std::stod(x.substr(19, 1));
    this->epoch_year   = std::stod(x.substr(21, 11));

    this->inclination  = std::stod(y.substr(9,  7));
    this->raan         = std::stod(y.substr(18, 7));
    this->perigree     = std::stod(y.substr(35, 7));
    this->anomaly      = std::stod(y.substr(44, 7));
    this->eccentricity = std::stod(y.substr(27, 6));
    this->mean_motion  = std::stod(y.substr(53, 10));
    this->b_drag       = std::stod(y.substr(54, 7));

}

bool checksum(std::string x, std::string y)
{
    int total = 0;
    //todo, what should
    for(int i = 0; i < TLE_CHECKSUM_IDX; i++)
    {
        if(std::isdigit(x[i])) total += stoi(x[i]);
        else if(std::isalpha(x[i]) || (x[i] == '+') || (x[i] == '.')) total += 0;
        else if(x[i] == '-') total += 1; 
    }
    if((total % 10) == (x[TLE_CHECKSUM_IDX]))
    {
        total = 0;
        for(auto j: y)
        {
            if(std::isdigit(j)) total += stoi(j);
            else if(std::isalpha(j) || (j == '+') || (j == '.')) total += 0;
            else if(j == '-') total += 1;       
        }   
        if(total % 10) == (y[TLE_CHECKSUM_IDX]) return true;
        else return false;
    }else return false;

    return false;
}

/*
    Epoch year and day- used for propogation time
    Inclination, RAAN, perigree, and anomaly - used for orbital orientation
    Mean motion - sets orbit size and period

    B_drag and eccentricity - 

*/


struct Entry
{
    int  entry_id;
    std::string satellite_name;
    int satellite_id;
    int transactions_count;
    std::string launch_number;
    std::string launch_year;
    std::string launch_piece;
    std::string epoch_day;
    std::string epoch_year;
    std::string ballistic_coefficient;
    std::string radiation_drag;
    std::string latitude;
    std::string longitude;
    std::string azimuth;
    std::string elevation;
    std::string ra;
    std::string declination;
    std::string timestamp;

    TLE_Element tle;

    Entry(int id, std::string s_n, int s_i, std::string ln, std::string ly, std::string lp,
          std::string ed, std::string ey, std::string bc, std::string rd, std::string lat, std::string log, std::string az, std::string el, std::string r, std::string dec, std::string tims) :
          entry_id(id), satellite_name(s_n), satellite_id(s_i), launch_number(ln),
          launch_year(ly), launch_piece(lp), epoch_day(ed), epoch_year(ey), ballistic_coefficient(bc),
          radiation_drag(rd), latitude(lat), longitude(log), azimuth(az), elevation(el), ra(r), declination(dec), timestamp(tims) {};

};

std::unordered_map<int, Entry>satellite_dictionary = {};

void to_json(json& j, Entry& p)
{
    j =json{{"satellite_id", p.satellite_id}, {"satellite_name", p.satellite_name}, {"launch_number", p.launch_number}, {"launch_year", p.launch_year}, {"launch_piece", p.launch_piece}, {"epoch_day", p.epoch_day}, {"epoch_year", p.epoch_year}
    , {"launch_piece", p.launch_piece}, {"epoch_day", p.epoch_day}, {"epoch_year", p.epoch_year}, {"ballistic_coefficient", p.ballistic_coefficient}, {"radiation_drag", p.radiation_drag}, {"latitude", p.latitude}, {"azimuth", p.azimuth}
    , {"elevation", p.elevation}, {"ra", p.ra}, {"declination", p.declination}, {"timestamp", p.timestamp}};
}

void from_json(const json& j, Entry& p)
{
    
    if(j.contains("error"))
    {
        std::cout << j.dump() << std::endl;
        //NOTE remember, eventually i want a FULL UI, this should also call some routine to update our UI
    }else
    {
        const auto &x = j.at("info");
        x.at("satid").get_to(p.satellite_id);
        x.at("satname").get_to(p.satellite_name);
        x.at("transactionscount").get_to(p.transactions_count);
        x.at("tle").get_to(p.tle);
    }
}

class SatelliteDictionary
{
    public:
    SatelliteDictionary(int ins);
    void readEntry(int entry);                                    //reads an entry from the dictionary
    Entry accessEntry(int entry);                         //accesses an object from the dictionary
    size_t write_callback(char * ptr, size_t size, size_t new_member, void* user_data);
    json getDataFromURL(std::string sid);
    void parse_tle(json j);
    void displayJSON(json j);
    protected:
    int instance;
    std::vector<Entry> user_satcat;
    json satcat;
    private:
    void generateSatCat();

};

SatelliteDictionary::SatelliteDictionary(int ins)
{
    std::cout << "Satellite Dictionary constructor called" << std::endl;
    this->instance = ins;
    std::cout << "constructor called" << std::endl;
    generateSatCat();
}

void SatelliteDictionary::parse_tle(json j)
{
    

    //so TLE is not individual JSON objects, it is a single tle object with many data members. It will require separate parsing
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


size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) 
{
    size_t total_size = size * nmemb;
    static_cast<std::string*>(userdata)->append(ptr, total_size);
    
    return total_size;
}

json SatelliteDictionary::getDataFromURL(std::string sid)
{
    CURL* curl = curl_easy_init();
    std::string body = "0";
    std::string api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";
    long status_code = 0;
   // return "https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (satellite_id, api_key);
    std::string s_url = "https://api.n2yo.com/rest/v1/satellite/tle/" + sid + "&apiKey=" + api_key;

    curl_easy_setopt(curl, CURLOPT_URL, s_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK || status_code != 200) return "";
    std::cout << body << std::endl;
    return json::parse(body);
    //returns a parsed JSON object - we then extract the values from n2yo's json value
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

void displayJSON(json j)
{
    for(auto& x: j)
    {
        j.dump();
        std::cout << std::endl;
    }
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
    std::cout << "DEBUG TEST BEGIN" << std::endl;
    try{
        displayJSON(d_tron->getDataFromURL("25544"));
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    std::cout << "DEBUG TEST END" << std::endl;
    while(1){}

    return 0;
}