#include "layout.h"
#include "libevdev/libevdev.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

Layout parse_layout(const std::filesystem::path &path, std::string model)
{
    if (model.empty())
    {
        std::ifstream board_name{"/sys/class/dmi/id/board_name"};
        std::getline(board_name, model);
    }
    std::ifstream db{path};
    bool model_found = false;
    std::vector<int> keys;
    uint8_t num_cols = 0;
    double top_offset = 0;

    std::regex ws_re{"\\s+"};
    while (db)
    {
        std::string line;
        std::getline(db, line);
        if (line.empty() or line[0] == '#')
            continue;
        if (line[0] == '[' and line.back() == ']')
        {
            if (model_found)
                break;
            num_cols = 0;
            top_offset = 0;
            keys.clear();
        }
        std::vector<std::string> el{std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),
                                    std::sregex_token_iterator()};
        if (el.size() > 2 and el[1] == "=")
        {
            if (el[0] == "match")
            {
                for (size_t i = 2; i < el.size(); ++i)
                {
                    if (el[i] == model)
                        model_found = true;
                }
            }
            else if (el[0] == "row")
            {
                if (num_cols == 0)
                    num_cols = el.size() - 2;
                else if (num_cols != el.size() - 2)
                {
                    throw std::runtime_error{"Malformed configuration for " + model + " in " + path.string() +
                                             ": all rows must have the same number of elements"};
                }
                for (size_t i = 2; i < el.size(); ++i)
                {
                    int code = libevdev_event_code_from_name(EV_KEY, el[i].c_str());
                    if (code == -1)
                    {
                        throw std::runtime_error{"Malformed configuration for " + model + " in " + path.string() +
                                                 ": unknown key " + el[i]};
                    }
                    keys.push_back(code);
                }
            }
            else if (el[0] == "top_offset")
            {
                top_offset = std::stod(el[2]);
            }
        }
    }
    if (not model_found)
        throw std::runtime_error{"No entry for " + model + " in " + path.string()};
    if (keys.empty() or num_cols == 0)
        throw std::runtime_error{"Invalid layout for " + model + " in " + path.string()};
    return Layout{keys, num_cols, top_offset};
}
