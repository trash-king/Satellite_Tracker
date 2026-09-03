#include <iostream>
#include <string>
//#include "cpr/cpr.h"
#include "sat_tracker.h"
#include "satellite_dict.h"


inline SatelliteDictionary sat_dict = SatelliteDictionary();

bool is_running  = true;

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    while(is_running)
    {


    }
    curl_global_cleanup();
    return 0;
}