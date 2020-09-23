#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <mutex>
#include <vector>

class CSV
{
public:
    enum class Mode
    {
        INOUT = 0,
        IN,
        OUT
    };

    CSV() = default;

    CSV(const std::string& _file_name, Mode _mode = Mode::OUT)
    {
        setFileName(_file_name);
        setMode(_mode);
        if(allowReading()) {
            read();
        }
    }

    void set(unsigned int row, unsigned int column, const std::string& value)
    {
        std::lock_guard<std::mutex> guard(table_m);

        if(row+1 > max_height) { max_height = row+1; }
        if(column+1 > max_width) { max_width = column+1; }

        auto& cell = table[row][column];
        if(write_protection) {
            if(cell == "") {
                cell = value;
            }
        } else {
            cell = value;
        }
        
    }
    
    void append(const std::map<unsigned int, std::map<unsigned int, std::string>>& other_table)
    {
        unsigned int row = max_height;
        unsigned int column = 0;
        for(const auto& r : other_table) {
            for(const auto& c : r.second) {
                set(row, column, c.second);
                column++;
            }
            row++;
            column = 0;
        }
        max_height = row;
    }

    void append(const CSV& csv)
    {
        auto other_table = csv.getTable();
        append(other_table);
    }

    void append(const std::string& file_n)
    {
        CSV file(file_n, Mode::IN);
        append(file.getTable());
    }

    void write()
    {
        if(allowWriting()) {

            if(file_name == "") {
                throw std::runtime_error("CSV file name cannot be blank if you want to write out.");
            }

            std::lock_guard<std::mutex> guard(file_m);
            std::ofstream file(file_name);

            if(file.is_open()) {
                for(int y = 0; y < max_height; ++y) {
                    for(int x = 0; x < max_width; ++x) {
                        file << table[y][x];
                        if(x < max_width-1) { file << ","; }
                    }
                    if(y < max_height-1){ file << "\n"; }
                }
                file.close();
            }
        } else {
            throw std::runtime_error("The current CSV file mode does not allow for writing.\n");
        }
    }

    void transpose()
    {
        std::map<unsigned int, std::map<unsigned int, std::string>> temp_table;
        unsigned int row = 0;
        unsigned int column = 0;
        for(const auto& r : table)
        {
            for(const auto& c : r.second)
            {
                temp_table[column][row] = c.second;
                column++;
            }
            row++;
            column = 0;
        }
        table = temp_table;
        unsigned int old_max_height = max_height;
        max_height = max_width;
        max_width = old_max_height;
    }

    void read()
    {
        if(allowReading())
        {
            if(file_name == "") {
                throw std::runtime_error("CSV file name cannot be blank if you want to read in a file.");
            }

            std::lock_guard<std::mutex> guard(file_m);
            std::ifstream file(file_name);
            if(file.is_open())
            {
                unsigned int row = 0;
                unsigned int column = 0;
                for(std::string line; std::getline(file, line);)
                {
                    std::vector<std::string> result;
                    std::istringstream iss(line);

                    for (std::string token; std::getline(iss, token, ','); )
                    {
                        set(row, column, token);
                        column++;
                    }
                    row++;
                    column = 0;
                }
                file.close();
            } else {
                throw std::runtime_error(file_name + " could not be opened for reading.");
            }
        } else {
          throw std::runtime_error("CSV file mode is not set to allow for reading.");  
        }
        
    }

    void clear()
    {
        for(auto& r : table) {
            r.second.clear();
        }
        table.clear();
    }

    const std::string& get(unsigned int row, unsigned int column)
    {
        return table[row][column];
    }

    const std::map<unsigned int, std::map<unsigned int, std::string>>& getTable() const
    {
        return table;
    }

    void enableOverwriteProtection()
    {
        write_protection = true;
    }

    void disableOverwriteProtection()
    {
        write_protection = false;
    }

    unsigned int getLastRow()
    {
        return max_height;
    }

    unsigned int getLastColumn()
    {
        return max_width;
    }

    void setFileName(const std::string& _file_name)
    {
        file_name = _file_name;
    }

    void setMode(Mode _mode)
    {
        mode = _mode;
    }

    bool allowReading()
    {
        return (mode == Mode::INOUT || mode == Mode::IN);
    }

    bool allowWriting()
    {
        return (mode == Mode::INOUT || mode == Mode::OUT);
    }

private:
    std::map<unsigned int, std::map<unsigned int, std::string>> table;
    std::string file_name   { "" };
    unsigned int max_height { 0 };
    unsigned int max_width  { 0 };
    Mode mode               { Mode::OUT };
    bool write_protection   { false };
    std::mutex table_m;
    std::mutex file_m;
};