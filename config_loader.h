#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <regex>
#include <cctype>

inline std::unordered_map<std::string, std::string> load_config_file(const std::string& file_name)
{
    std::unordered_map<std::string, std::string> output;
    std::ifstream file(file_name);

    if(file.is_open()) {

        // Load in the entire file
        std::string file_data;
        file.seekg(0, std::ios::end);
        unsigned long file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        file_data.resize(file_size);
        file.read((char*)file_data.data(), file_size);

        // Clean the config file of unwanted characters.
        std::regex good_characters("([^a-zA-Z0-9:._\r\n-]+)");
        std::istringstream cleaned_file_data;
        cleaned_file_data.str(std::regex_replace(file_data, good_characters, ""));

        std::string line;
        while(std::getline(cleaned_file_data, line)) {
            if(line.find(':') > 0) { // Check to see if a colon exists

                // Explode the line around the colon
                std::vector<std::string> e;
                std::istringstream iss(line);
                for (std::string token; std::getline(iss, token, ':'); ) {
                    e.push_back(std::move(token));
                }

                if(e.size() == 2) { // Check to make sure there is only two elements
                    if(e.at(0).size() > 0 && e.at(1).size() > 0) { // Check to see if there is something writen on eather side of the colon
                        output.emplace(e.at(0), e.at(1));
                    }
                }
            }
        }
    } else {
        throw std::runtime_error("Failed to open the config file: " + file_name);
    }
    return output;
}

/*

Config files must be in the following format:

name1:value1
name2:value2
name3:value3

After loading you can access it by using the name of the parameters:

auto config = load_config_file("config");
config["name1"]
The value will be in a string and can be converted to whatever value you wish.
There is obviously no type verifying. USE AT YOUR OWN RISK.

*/