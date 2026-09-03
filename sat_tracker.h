#include <cmath>
#include <curl/curl.h>
#include <string>
#include <iostream>

class SatTracker
{
    public:
    SatTracker();
    void selectSatellite();
    json getDataFromURL();
    void getSatelliteData();

    void displayLaunchData();
    void displayEpochData();
    void displayOrbitalData();

    //"https://api.n2yo.com/rest/v1/satellite/tle/%s&apiKey=%s" % (tracked_satellite.satellite_id, tracked_satellite.satellite_api_key)
    protected:
    char * getLaunchData();      //returns a data object instead of displaying data     
    char * getEpochData();
    char * getOrbitalData();
    static size_t write_callback(char * ptr, size_t size, size_t new_member, void* user_data);
    
    const double gec = 1.082626 * std::pow(10, -3); //geopotential coefficient
    const double eqr = 6378.137;                    //Earth's equatorial radius
    private:
    const char * api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";
    void propagateOrbit();

};