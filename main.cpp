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
#define  SAT_ID_LENGTH            5

#define  TLE_PLACEHOLDER          "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

using namespace nlohmann;

bool is_running  = true;

struct TLE_Element
{
    TLE_Element(std::string t);
    public:

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

TLE_Element::TLE_Element(std::string t)
{
    std::string x = t.substr(0,69);
    std::string y = t.substr(72,141);

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
    size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata);
    json get_sat_request(){return (*curl_return);}
    void getDataFromURL(std::string sid);
    bool checksum();
    protected:
    
    private:
    std::string api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";
    json * curl_return;
};

RequestManager::RequestManager()
{
    printf("RequestManager Instantiated\n");
}

size_t RequestManager::write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) 
{
    size_t total_size = size * nmemb;
    static_cast<std::string*>(userdata)->append(ptr, total_size);
    
    return total_size;
}

void RequestManager::getDataFromURL(std::string sid)
{
    CURL* curl = curl_easy_init();
    std::string body = "";
   
    long status_code = 0;
   // return "https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (satellite_id, api_key);
    std::string s_url = "https://api.n2yo.com/rest/v1/satellite/tle/" + sid + "&apiKey=" + api_key;

    curl_easy_setopt(curl, CURLOPT_URL, s_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK || status_code != 200)
    {
        curl_easy_strerror(res);
    }
    *curl_return = json::parse(body); 
    if(curl_return->contains("error")) printf("Error: %s\n",curl_return->at("error"));
    //return json::parse(body);

    //returns a parsed JSON object - we then extract the values from n2yo's json value
}

bool RequestManager::checksum()
{
    std::string z = this->curl_return->at("tle"); //we don't need to store a whole string, get rid of this when it works
    std::string x = z.substr(0,69);
    std::string y = z.substr(72, 141);

    int total = 0;
       
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

RequestManager * request_manager = new RequestManager();
extern RequestManager * request_manager;

struct Entry
{
    int entry_id;
    int satellite_id;
    int transactions_count;

    TLE_Element tle = TLE_Element(TLE_PLACEHOLDER);

    std::string satellite_name;
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
};
/*
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
        x.at("tle").get_to(p.t_string);
    }
}
*/


std::unordered_map<int, Entry>satellite_dictionary = {};

class SatelliteDictionary
{
    public:
    SatelliteDictionary(int ins);
    void readEntry(int entry);                                    //reads an entry from the dictionary
    Entry accessEntry(int entry);                         //accesses an object from the dictionary
    bool createEntry(std::string id);                    //creates a new entry entirely
    void populate_entry(Entry * entry);
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

void SatelliteDictionary::populate_entry(Entry * entry)
{
    std::string s = entry->tle.tle;

    entry->launch_year = s.substr(9,2);
    entry->launch_number = s.substr(11,3);
    entry->launch_piece = s.substr(14,3);

    entry->epoch_year = s.substr(19,1);
    entry->epoch_day  = s.substr(21,11);

    entry->ballistic_coefficient = s.substr(33,10);
    entry->radiation_drag        = s.substr(53,8);
    /*
          self.ballistic_coefficient = launchData[33:43]
        self.radiation_drag = launchData[53:61]
    */
}

void SatelliteDictionary::propogate_tle(TLE_Element tle)
{
    /*
    double radius = radius_of_curvature(tle.)
    */
   std::cout << "null" << std::endl;
}

bool SatelliteDictionary::createEntry(std::string id)
{
    if(request_manager == NULL)
    {
        printf("Error: Request Manager uninstantiated:\n");
        return false;
    }
    request_manager->getDataFromURL(id);
    Entry new_entry(request_manager->get_sat_request());
    
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

SatTracker * s_tron              = new SatTracker();

int main()
{
    bool running = true;
    std::string input = "";
    while(running)
    {
        try{
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
            if(input.size() < SAT_ID_LENGTH)
            {
                while(input.size() < SAT_ID_LENGTH) input.insert(0, 1, '0');
            }
            std::cout << "getdatafromurl(input)\n" << std::endl;
            request_manager->getDataFromURL(input);
        }catch(const std::runtime_error& e)
        {
            std::cerr << "Error: " << e.what() << '\n';
        }catch (...) 
        {
        // Catches EVERYTHING else (ints, custom non-std types, etc.)
        std::cerr << "An unknown C++ exception occurred.\n";
        }   
    }
    std::cout << "DEBUG TEST END" << std::endl;

    return 0;
}