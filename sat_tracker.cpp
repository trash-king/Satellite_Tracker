#include "sat_tracker.h"


SatTracker::SatTracker()
{

}

void SatTracker::selectSatellite()
{

}

static size_t SatTracker::write_callback(char * ptr, size_t size, size_t new_member, void* user_data)
{
    auto output = static_cast<std::string*>(user_data);
    output->append(ptr, size * new_member);
    return size  * new_member;
}

json SatTracker::getDataFromURL()
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

void SatTracker::getSatelliteData(char * satellite_id)
{
    json outputData = getDataFromURL();
    char res;
    std::cout << "Save to Local Dictionary? Y/N"<< std::endl;
    cin >> res;
    if(res == "Y") SatelliteDictionary.appendToUserDict(outputData);

}