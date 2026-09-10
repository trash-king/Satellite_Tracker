#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
//#include "cpr/cpr.h"
#include "sat_tracker.h"

using namespace nlohmann;

SatelliteDictionary sat_dict    = SatelliteDictionary();
SatTracker          sat_track   = SatTracker();


bool is_running  = true;

int main()
{
    SatTracker().getSatelliteData();
#if 0
    //curl_global_init(CURL_GLOBAL_DEFAULT);
    while(is_running)
    {


    }
    //curl_global_cleanup();
    return 0;
#endif
    return 0;
}