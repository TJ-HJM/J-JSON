#include "parser.h"
#include <stdexcept>

namespace json {

Parser::Parser() : str_(""), index_(0) {}
void Parser::load(const string &str) {
    str_ = str;
    index_ = 0;
}

void Parser::skip_white_space(){
    while (str_[index_] == ' ' || str_[index_] == '\n' || str_[index_] == '\r' || str_[index_] == '\t') {
        index_++;
    }
}
char Parser::get_next_token(){
    skip_white_space();
    return str_[index_++];
}

Json Parser::parse() {
    char ch = get_next_token();
    switch (ch)
    {
    case 'n':
        return parse_null();
    case 't':
    case 'f':
        return parse_bool();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return parse_number();
    case '"':
        return Json(parser_string());
    case '[':
        return parser_array();
    case '{':
        return parser_object();
    default:
        break;
    }
    throw std::logic_error("unexpected char");
}

Json Parser::parse_null() {
    index_--;
    if (str_.compare(index_, 4, "null") == 0)
    {
        index_ += 4;
        return Json();
    }
    throw std::logic_error("parse null error");
}
Json Parser::parse_bool() {
    index_--;
    if (str_.compare(index_, 4, "true") == 0)
    {
        index_ += 4;
        return Json(true);
    }
    else if (str_.compare(index_, 5, "false") == 0) {
        index_ += 5;
        return Json(false);
    }
    throw new std::logic_error("parse bool error");
}
Json Parser::parse_number() {
    index_--;
    int pos = index_;
    if (str_[index_] == '-')
        index_++;
    if (str_[index_] < '0' || str_[index_] > '9')   // 数字必须以0~9开头
        throw std::logic_error("parse number error");
    while (str_[index_] >= '0' && str_[index_] <= '9') {
        index_++;
    }
    if (str_[index_] != '.') {
        int i = std::atoi(str_.c_str() + pos);  // c_str()函数？
        return Json(i);
    }
    // 发现小数点
    index_++;
    if (str_[index_] < '0' || str_[index_] > '9')   // 小数点后面必须以0~9开头
        throw std::logic_error("parse number error");
    while (str_[index_] >= '0' && str_[index_] <= '9') {
        index_++;
    }
    double f = std::atof(str_.c_str() + pos);
    return Json(f);
}
string Parser::parser_string() {
    string out;
    while (true) {
        char ch = str_[index_++];   // 要保留字符串中的空白字符
        if (ch == '"')
            break;
        if (ch == '\\') {
            ch = str_[index_++];
            switch (ch)
            {
            case 'n':
                out += '\n';
                break;
            case 'r':
                out += '\r';
                break;
            case 't':
                out += '\t';
                break;
            case 'b':
                out += '\b';
                break;
            case 'f':
                out += '\f';
                break;
            case '"':
                out += "\"";
                break;
            case '\\':
                out += "\\";
                break;
            case '/':
                out += "/";
                break;
            case 'u':
                out += "\\u";
                for (int i = 0; i < 4; i++)
                {
                    out += str_[index_++];
                }
                break;
            default:
                throw std::logic_error("parser string error");
                break;
            }
        }
        else {
            out += ch;
        }
    }
    return out;
}
Json Parser::parser_array() {
    index_--;                   // 思考为什么这里要后退一个字符？
    Json arr(Json::json_array);
    char ch = get_next_token();
    if (ch == ']') {
        return arr;
    }
    while (true) {
        arr.append(parse());    // 解析出一个对象就加入数组中
        ch = get_next_token();  // 对于对象后面的字符进行判断
        if (ch == ']') {        // 若一个Json对象后面是一个 ] 代表数组结束了
            break;
        }
        if (ch != ',') {        // 若一个Json对象后面既不是 ] 也不是 ， 代表格式出错
            throw new std::logic_error("parse array error");
        }
        // 若ch == ',' 则继续解析写一个对象
    }
    return arr;
}
Json Parser::parser_object() {
    index_--;
    Json obj(Json::json_object);
    char ch = get_next_token();
    if (ch == '}')
        return obj;
    while (true) {
        ch = get_next_token();
        // 先解析key
        if (ch != '"') {
            throw new std::logic_error("parse object error");
        }
        string key = parser_string();
        ch = get_next_token();
        // key:value
        if (ch != ':') {
            throw new std::logic_error("parse object error");
        }
        // 解析value
        obj[key] = parse();
        ch = get_next_token();
        if (ch == '}') {
            break;
        }
        if (ch != ',') {
            throw new std::logic_error("parse object error");
        }
    }
    return obj;
}
}