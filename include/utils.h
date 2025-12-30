#pragma once

#include <string>

std::string read_file(std::string filename);

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !std::isspace(ch); }).base(),
            s.end());
}

// trim from both ends (copy)
static inline std::string trim(std::string s) {
    ltrim(s);
    rtrim(s);
    return s;
}

static inline std::string consume_token(std::string& s) {
    size_t pos = 0;
    while (pos < s.size() && !isspace(s[pos]) && s[pos] != 'x' && s[pos] != '^')
        ++pos;
    std::string token = s.substr(0, pos);
    s = s.substr(pos);
    return token;
}

std::vector<std::string> split(const std::string& s, char delimiter);