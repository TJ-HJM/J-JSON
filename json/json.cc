#include <stdexcept>    // #include <exception> 也可以？？
using std::logic_error;
using std::out_of_range;

#include "json.h"

#include <sstream>
using std::stringstream;

#include "../parser/parser.h"

// using namespace json;
namespace json {
// 命名空间json中各元素的实现
Json::Json() : type_(json_null) {}

Json::Json(bool value) : type_(json_bool) {
    val_.bool_val = value;
}

Json::Json(int value) : type_(json_int) {
    val_.int_val = value;
}

Json::Json(double value) : type_(json_double) {
    val_.double_val = value;
}

Json::Json(const char *value) : type_(json_string) {
    val_.string_val = new std::string(value);
}

Json::Json(const string & value) : type_(json_string)
{
    val_.string_val = new std::string(value);
}

Json::Json(Type type) : type_(type) {
    switch (type_)
    {
    case json_null:
        break;
    case json_bool:
        val_.bool_val = false;
        break;
    case json_int:
        val_.int_val = 0;
        break;
    case json_double:
        val_.double_val = 0.0;
        break;
    case json_string:
        val_.string_val = new std::string;
        break;
    case json_array:
        val_.array_val = new std::vector<Json>;
        break;
    case json_object:
        val_.object_val = new std::map<std::string, Json>;
        break;

    default:
        break;
    }
}
// 拷贝构造函数
Json::Json(const Json &other) {
    copy(other);
}
// 拷贝赋值运算符
Json &Json::operator=(const Json &other) {
    clear();    // 先把等号左边对象清空，释放原来对象的动态内存，避免内存泄漏
    copy(other);
}// 但是拷贝过程中，还是存在多个指针指向同一个动态内存中的对象

void Json::copy(const Json &other) {
    type_ = other.type_;
    switch (type_)
    {
    case json_null:
        break;
    case json_bool:
        val_.bool_val = other.val_.bool_val;
        break;
    case json_int:
        val_.int_val = other.val_.int_val;
        break;
    case json_double:
        val_.double_val = other.val_.double_val;
        break;
    case json_string:
        val_.string_val = other.val_.string_val;    // 浅拷贝：只拷贝了指针
        break;
    case json_array:
        val_.array_val = other.val_.array_val;      // 优缺点？怎么实现深拷贝？ 考虑智能指针？引用计数？
        break;
    case json_object:
        val_.object_val = other.val_.object_val;
        break;

    default:
        break;
    }
}

// 释放对象内存, 清空对象
void Json::clear() {
    switch (type_)
    {
    case json_null:
        break;
    case json_bool:
        val_.bool_val = false;
        break;
    case json_int:
        val_.int_val = 0;
        break;
    case json_double:
        val_.double_val = 0.0;
        break;
    case json_string:
    {
        delete val_.string_val;
        break;
    }
    case json_array:
    {
        for (auto it = (val_.array_val)->begin(); it != (val_.array_val)->end(); ++it)
            it->clear();
        delete val_.array_val;
        break;
    }
    case json_object:
    {
        for (auto it = (val_.object_val)->begin(); it != (val_.object_val)->end(); ++it)
            (it->second).clear();
        delete val_.object_val;
        break;
    }
    default:
        break;
    }
    type_ = json_null;
}

// 类型转换函数
Json::operator bool() {
    if (type_ != json_bool) {
        throw logic_error("type error, not bool value"); // throw new std::logic_error("type error, not bool value");
    }
    return val_.bool_val;
}
Json::operator int() {
    if (type_ != json_int) {
        throw logic_error("type error, not int value");
    }
    return val_.int_val;
}
Json::operator double() {
    if (type_ != json_double) {
        throw logic_error("type error, not double value");
    }
    return val_.double_val;
}
Json::operator string() {
    if (type_ != json_string) {
        throw logic_error("type error, not string value");
    }
    return *(val_.string_val);
}
// 显式的类型转换
bool Json::asBool() const {
    if (type_ != json_bool) {
        throw new logic_error("type error, not bool value");
    }
    return val_.bool_val;
}
int Json::asInt() const {
    if (type_ != json_int) {
        throw new logic_error("type error, not int value");
    }
    return val_.int_val;
}
double Json::asDouble() const {
    if (type_ != json_double) {
        throw new logic_error("type error, not double value");
    }
    return val_.double_val;
}
string Json::asString() const {
    if (type_ != json_string) {
        throw new logic_error("type error, not string value");
    }
    return *(val_.string_val);
}

// 数组数据的接口实现
Json &Json::operator [] (int index) {
    if (type_ != json_array) {
        clear();
        type_ = json_array;
        val_.array_val = new vector<Json>;
    }
    if (index < 0) {
        throw out_of_range("index can not smaller than zero");
    }
    if ( index >= (val_.array_val)->size() ) {// 扩容是用于赋值操作
        // increase capicity
        for (int i = (val_.array_val)->size(); i <= index; ++i) {
            (val_.array_val)->push_back(Json());
        }
    }
    return (*(val_.array_val))[index]; // (val_.array_val)->at(index);
}
void Json::append(const Json &other) {
    if (type_ != json_array) {// 不管原来是什么类型的Json值，都直接强制变成数组, 这样是否合理？？？
        clear();
        type_ = json_array;
        val_.array_val = new vector<Json>();
    }
    (val_.array_val)->push_back(other);
}

// 对象数据的接口实现
Json &Json::operator[](const char *key) {
    string name(key);
    return (*(this))[name];    // 调用参数为string类型的重载版本
}
Json &Json::operator[](const string &key) {
    if (type_ != json_object) {
        clear();
        type_ = json_object;
        val_.object_val = new map<string, Json>;
    }
    // 不需要判断key不在对象（map）中吗？map对象在调用[]时会自动创建
    // 也不需要判断对象是否为object_val类型吗?
    return (*(val_.object_val))[key];
}

bool Json::operator==(const Json &other) {
    if (type_ != other.type_)
        return false;
    switch (type_)
    {
    case json_null:
        return true;
        // break;
    case json_bool:
        return val_.bool_val == other.val_.bool_val;
    case json_int:
        return val_.int_val == other.val_.int_val;
    case json_double:
        return val_.double_val == other.val_.double_val;
    case json_string:
        return *(val_.string_val) == *(other.val_.string_val);
    // 数组和对象只简单判断了指针是否相等
    case json_array:
        return val_.array_val == other.val_.array_val;
    case json_object:
        return val_.object_val == other.val_.object_val;
    default:
        break;
    }
    return false;   // 这一句是什么情况下发生？
}
bool Json::operator!=(const Json &other) {
    return !((*this) == other);
}

// 将Json类对象以string形式返回
string Json::str() const {
    stringstream ss;
    switch (type_)
    {
    case json_null:
        ss << "null";
        break;
    case json_bool:
        if (val_.bool_val)
            ss << "true";
        else
            ss << "false";
        break;
    case json_int:
        ss << val_.int_val;
        break;
    case json_double:
        ss << val_.double_val;
        break;
    case json_string:
        ss << '\"' << *(val_.string_val) << '\"';   // Json中的字符串需要由" " 括起来
        break;
    case json_array:
        {
            ss << '[';  // 可以用于存储单个字符
            // for ( int i = 0; i < (val_.array_val)->size(); ++i ) {
            // }
            for (auto it = (val_.array_val)->cbegin(); it != (val_.array_val)->cend(); it++) {
                if (it != val_.array_val->cbegin())
                    ss << ',';      // JSON数组中的值使用 ，分隔
                ss << it->str();    // 递归调用str()，解析数组中的每个值
            }
            ss << ']';
            break;
        }// 什么情况下需要添加{}
    case json_object:
        {
            ss << '{';
            for (auto it = (val_.object_val)->cbegin(); it != (val_.object_val)->cend(); it++) {
                // it 指向map中的pair
                if (it != (val_.object_val)->cbegin())
                    ss << ',';      // JSON数组中的值使用 ，分隔
                ss << '\"' << it->first << '\"' << ":" << it->second.str();    // 递归调用str()，解析数组中的每个值
            }
            ss << '}';
            break;
        }
    }
    return ss.str();
}

// 判断数组中是否含有某个索引，对象中是否含有某个key
bool Json::has(int index) {
    if (type_ != json_array)
            return false;
    int size = val_.array_val->size();
    return (index >= 0 && index < size);
}
bool Json::has(const char *key) {
    string name(key);
    return has(name);
}
bool Json::has(const string &key) {
    if (type_ != json_object)
        return false;
    return (val_.object_val)->find(key) != val_.object_val->end();
}

// 删除操作
void Json::remove(int index) {
    if (!has(index))
        return;
    (*(val_.array_val))[index].clear();    // 注意要先释放数组中Json数据的内存 // (val_.array_val)->at(index).clear();
    val_.array_val->erase(val_.array_val->begin() + index);   // val_.array_val->erase(begin() + index);
}
void Json::remove(const char *key) {
    string name(key);
    remove(name);
}
void Json::remove(const string &key) {
    if (!has(key))
        return;
    (*(val_.object_val))[key].clear();
    (val_.object_val)->erase(key);
}

// 解析字符串函数
void Json::parse(const string &str) {
    // 通过parser类解析字符串
    Parser p;
    p.load(str);
    *this = p.parse();
}

}