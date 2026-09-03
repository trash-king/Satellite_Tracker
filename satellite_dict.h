#include "nlohmann/json.hpp"
#include <vector>
using json = nlohmann::json;

struct Entry
{
    int  entry_id;
    char * satellite_name;
    char * satellite_id;
    char * launch_number;
    char * launch_year;
    char * launch_piece;
    char * epoch_day;
    char * epoch_year;
    char * ballistic_coefficient;
    char * radiation_drag;

    Entry(int id, char * s_n, char * s_i, char * ln, char * ly, char * lp
          char * ed, char * ey, char * bc, char * rd) :
          entry_id(id), satellite_name(s_n), satellite_id(s_i), launch_number(ln),
          launch_year(ly), launch_piece(lp), epoch_day(ed), epoch_year(ey), ballistic_coefficient(bc),
          radiation_drag(rd) {}

};

class SatelliteDictionary
{
    public:
    SatelliteDictionary();
    void readEntry();                       //reads an entry from the dictionary
    json accessEntry();                     //accesses a JSON object from the dictionary
    void appendToUserDict(json * entry);
    protected:
    std::vector<Entry> user_satcat;
    json satcat;
    private:
    void generateSatCat();

}