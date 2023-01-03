#ifndef JSON_H  // #pragma once
#define JSON_H
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

namespace json
{

    class Json
    {
    public:
        enum Type
        { // JSON中的数据类型
            json_null = 1,
            json_bool,
            json_int,
            json_double,
            json_string,
            json_array,
            json_object
        };
        // 默认构造
        Json();
        // 单个参数的构造函数
        Json(bool value);
        Json(int value);
        Json(double value);
        Json(const char *value);
        Json(const string &value);
        Json(const vector<Json> &value);
        Json(const map<string, Json> &value);
        Json(Type type);
        void clear();
        void copy(const Json &other);
        // 拷贝构造函数
        Json(const Json &other);
        // 拷贝赋值运算符
        Json &operator=(const Json &other);

        // Json类型向基本类型转换
        operator bool();
        operator int();
        operator double();
        operator string();
        // 显式的类型转换
        bool asBool() const;
        int asInt() const;
        double asDouble() const;
        string asString() const;

        // 关系运算符重载
        bool operator==(const Json &other);
        bool operator!=(const Json &other);
        
        // 类型判断
        bool isNull() const { return type_ == json_null; }
        bool isBool() const { return type_ == json_bool; }
        bool isInt() const { return type_ == json_int; }
        bool isdouble() const { return type_ == json_double; }
        bool isString() const { return type_ == json_string; }
        bool isArray() const { return type_ == json_array; }
        bool isObject() const { return type_ == json_object; }
        
        // 获取JSON类的内容，以字符串形式返回
        string str() const;

        // 数组的[]重载
        Json &operator[](int index);
        // 数组尾部添加元素
        void append(const Json &other);
        // 数组的迭代器：用于遍历数组数据
        typedef vector<Json>::iterator iterator;
        iterator begin() {
            return val_.array_val->begin();
        }
        iterator end() {
            return val_.array_val->end();
        }

        // 对象的[]重载
        Json &operator[](const char *key);
        Json &operator[](const string &key);

        // 判断数组中是否含有某个索引，对象中是否含有某个key
        bool has(int index);
        bool has(const char *key);
        bool has(const string &key);

        // 删除操作
        void remove(int index);
        void remove(const char *key);
        void remove(const string &key);

        // 解析字符串函数
        void parse(const string &str);

    private:
        // 联合体Value, 用于存储JSON的值
        union Value
        {
            bool bool_val;
            int int_val;
            double double_val;
            // 容器和string是可变大小的，不能直接放在union里，故使用指针（大小固定）
            string *string_val;
            vector<Json> *array_val;
            map<string, Json> *object_val;
        };
        // 一个Json值：类型、值
        Type type_;
        Value val_;
    };
}
// API 设计： 为什么要这么设计，怎么想到设计这些API，背后的思考逻辑是什么？
#endif