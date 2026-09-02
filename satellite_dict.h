#include "nlohmann/json.hpp"
using json = nlohmann::json;

class SatelliteDictionary
{
    public:
    SatelliteDictionary();
    void readEntry();           //reads an entry from the dictionary
    json accessEntry();         //accesses a JSON object from the dictionary
    protected:
    json satcat;
    private:
    const char * api_key = "6M9QZG-6FVM3G-L9BFLH-5TYH";
    void generateSatCat();

}