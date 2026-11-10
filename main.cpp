#include <iostream>
#include <ostream>
#include <istream>
#include <unordered_map>
#include <fstream>
#include <stdexcept>
#include <cctype>

#include "orbital_math.h"
#include "SGP4.h"
#include "nlohmann/json.hpp"

#include <curl/curl.h>
#define  TLE_CHECKSUM_IDX         69

using namespace nlohmann;

bool is_running  = true;

struct TLE_Element
{
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
        if(std::isdigit(x[i])) total += x[i] - '0';
        else if(std::isalpha(x[i]) || (x[i] == '+') || (x[i] == '.')) total += 0;
        else if(x[i] == '-') total += 1; 
    }
    if((total % 10) == (x[TLE_CHECKSUM_IDX]))
    {
        total = 0;
        for(auto j: y)
        {
            if(std::isdigit(j)) total += j - '0';
            else if(std::isalpha(j) || (j == '+') || (j == '.')) total += 0;
            else if(j == '-') total += 1;       
        }   
        if((total % 10) == (y[TLE_CHECKSUM_IDX])) return true;
        else return false;
    }else return false;

    return false;
    }

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
};

/*
    Epoch year and day- used for propogation time
    Inclination, RAAN, perigree, and anomaly - used for orbital orientation
    Mean motion - sets orbit size and period

    B_drag and eccentricity - 

*/
   
/*

Our procedure -
1: Make call to N2Yo
2: populate Entry with our recieved JSON object
3: populate TLE_Element of Entry with nav data
4: Update with new nav data information

*/

//delegate connectivity to a separate manager, not the dictionary

class RequestManager
{
    public: 
    RequestManager();
    size_t write_callback(char * ptr, size_t size, size_t new_member, void* user_data);
    json get_sat_request(){return (*curl_return);}
    protected:
    void getDataFromURL(std::string sid);
    private:
    std::string api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";
    json * curl_return;
}

RequestManager::RequestManager()
{

}

size_t RequestManager::write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) 
{
    size_t total_size = size * nmemb;
    static_cast<std::string*>(userdata)->append(ptr, total_size);
    
    return total_size;
}

json RequestManager::getDataFromURL(std::string sid)
{
    CURL* curl = curl_easy_init();
    std::string body = "";
   
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

    if(res != CURLE_OK || status_code != 200)
    {
        curl_easy_strerror(res);
        return "";
    }
    &curl_return = json::parse(body); 
    //return json::parse(body);

    //returns a parsed JSON object - we then extract the values from n2yo's json value
}

std::unordered_map<int, Entry>satellite_dictionary = {};

struct Entry
{
    public:
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
    std::string t_string; //we extract T-String from the tle element string.
    //THere's a better way of doing this but I'm doing this for now
/*
    Entry(int id, std::string s_n, int s_i, std::string ln, std::string ly, std::string lp,
          std::string ed, std::string ey, std::string bc, std::string rd, std::string lat, std::string log, std::string az, std::string el, std::string r, std::string dec, std::string tims, TLE_Element t) :
          entry_id(id), satellite_name(s_n), satellite_id(s_i), launch_number(ln),
          launch_year(ly), launch_piece(lp), epoch_day(ed), epoch_year(ey), ballistic_coefficient(bc),
          radiation_drag(rd), latitude(lat), longitude(log), azimuth(az), elevation(el), ra(r), declination(dec), timestamp(tims), tle(t) {};
*/
    Entry(json j);
    void populate_entry(std::string s);
    void extract_t_string();


};

Entry::Entry(json j)
{
    
}

void Entry::to_json() const {
    return json{
        {"satellite_id", satellite_id},
        {"satellite_name", satellite_name},
        {"launch_number", launch_number},
        {"launch_year", launch_year},
        {"launch_piece", launch_piece},
        {"epoch_day", epoch_day},
        {"ballistic_coefficient", ballistic_coefficient},
        {"radiation_drag", radiation_drag},
        {"latitude", latitude},
        {"azimuth", azimuth},
        {"elevation", elevation},
        {"ra", ra},
        {"declination", declination},
        {"timestamp", timestamp}
    }
}

void Entry::from_json(const json& j, Entry& p)
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
        x.at("tle").get_to(p.t_string);
    }
}

void Entry::extract_t_string()
{

}

void Entry::populate_entry()
{
    std::string s = this->t_string;

    this->launch_year = s.substr(9,2);
    this->launch_number = s.substr(11,3);
    this->launch_piece = s.substr(14,3);

    this->epoch_year = s.substr(19,1);
    this->epoch_day  = s.substr(21,11);

    this->ballistic_coefficient = s.substr(33,10);
    this->radiation_drag        = s.substr(53,8);
    /*
          self.ballistic_coefficient = launchData[33:43]
        self.radiation_drag = launchData[53:61]
    */
}

class SatelliteDictionary
{
    public:
    SatelliteDictionary(int ins);
    void readEntry(int entry);                                    //reads an entry from the dictionary
    Entry accessEntry(int entry);                         //accesses an object from the dictionary
    bool createEntry(std::string id);                    //creates a new entry entirely

    void propogate_tle(TLE_Element tle);    //makes the calls to orbital_math
    void displayJSON(json j);
    protected:
    int instance;

    std::vector<Entry> user_satcat;
    TLE_Element * current_element = nullptr;            //calls to Orbital Math occur one tle call at a time, we don't need an array of TLE's but one which is constantly updated.

    std::unordered_map<TLE_Element*, int> user_satellite_map;   //this contains all recently tracked objects,sorted by ID and most recently updated tle.

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

void SatelliteDictionary::propogate_tle(TLE_Element tle)
{
    /*
    double radius = radius_of_curvature(tle.)
    */
   std::cout << "null" << std::endl;
}

void SatelliteDictionary::createEntry(std::string id)
{
    Entry new_entry(getDataFromURL(id));
    int cur_id = satellite_dictionary.size() + 1;

    auto [iterator, success] = satellite_dictionary.emplace(cur_id, new_entry);
    if(!success)
    {
        printf("Failure to add Entry %s at position %d\n",id, cur_id);
        return false;
    }else
    {
        printf("New Entry %s added at position %d\n",id, cur_id);
        readEntry(cur_id);
        return true;
    }

    printf("New Entry %s failed for unknown reason.\n",id);
    return false;
    
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
    bool running = true;
    std::string input = "";
    while(running)
    {
        std::cout << "Enter NORAD satellite ID: " << std::endl;
        std::cin >> input;
        for(auto x: input)
        {
            if(!std::isdigit(x))
            {
                std::cout << "Invalid ID" << std::endl;
                input = "";
                break;
            }
        }
        while(input.size() > 5) input.insert(0, 1, '0');

        displayJSON(d_tron->getDataFromURL(input));
    }
    std::cout << "DEBUG TEST END" << std::endl;

    return 0;
}