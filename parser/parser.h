#pragma once
#include <string>
using std::string;
#include "../json/json.h"

namespace json {

class Parser
{
public:
    Parser();
    void load(const string &str);

    Json parse();

private:
    void skip_white_space();
    char get_next_token();
    
    Json parse_null();
    Json parse_bool();
    Json parse_number();
    string parser_string();
    Json parser_array();
    Json parser_object();

private:
    string str_;
    int index_;

};

}