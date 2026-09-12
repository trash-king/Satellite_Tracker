#pragma once
#include <vector>

#include "nlohmann/json.hpp"
using namespace nlohmann;

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

class SatelliteDictionary
{
    public:
    SatelliteDictionary(int ins);
    void readEntry(int entry);                                    //reads an entry from the dictionary
    Entry accessEntry(int entry);                         //accesses an object from the dictionary
    static size_t write_callback(char * ptr, size_t size, size_t new_member, void* user_data);
    void getSatelliteData(std::string satellite_id);
    json getDataFromURL();
    void appendToUserDict(json * entry);
    protected:
    int instance;
    std::vector<Entry> user_satcat;
    json satcat;
    private:
    void generateSatCat();

};