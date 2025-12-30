#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "../include/utils.h"

std::string read_file(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the whole file into the stringstream
    return buffer.str();     // Convert stringstream to string
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        std::string token = trim(s.substr(start, end - start));
        if(!token.empty()) tokens.push_back(token);
        start = end + 1;
        end = s.find(delimiter, start);
    }

    // last token
    std::string token = trim(s.substr(start));
    if(!token.empty()) tokens.push_back(token);

    return tokens;
}