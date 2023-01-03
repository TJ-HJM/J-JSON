#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "json/json.h"
using namespace json;

void test_json();
void test_parser();
void test();

int main()
{
    // test_json();
    // test_parser();
    test();

    return 0;
}

void test_json() {
        // 创建对象：需要构造函数

    // 基本数据类型的使用
    Json v1;
    Json v2 = true;
    Json v3 = 123;
    Json v4 = 1.23;
    Json v5 = "hello world";
    Json v6 = v5;

    // 数组类型的使用
    // 若没有创建拷贝赋值运算符时，这里用的是默认拷贝赋值运算符
    // constexpr json::Json &json::Json::operator=(const json::Json &)
    Json arr;
    arr[0] = true;
    arr[1] = 2;
    arr[2] = "string";
    arr[3] = v5;
    arr.append("hello world");
    bool b = arr[0];
    int i = arr[1];
    const string &s = arr[2];
    // int err = arr[3];   // 这里会抛出异常

    // 数组迭代器测试
    for (auto it = arr.begin(); it != arr.end(); it++) {
        cout << it->str() << endl;
    }

    cout << arr.has(0) << endl;
    cout << arr.has(5) << endl;

    // 获取JSON数据，以字符串形式输出
    cout << arr.str() << endl;

    // 对象类型的使用
    Json obj;
    obj["bool"] = false;
    obj["int"] = 1;
    obj["arr"] = arr;
    obj["str"] = "hello world"; // 这些键值对在map中顺序不确定
    cout << obj.has("bool") << endl;
    cout << obj.has("out") << endl;
    cout << obj.str() << endl;
    obj.remove("bool");
    cout << obj.str() << endl;

    obj.clear();        // 使用完后手动释放, 会产生野指针！
    // arr.clear();     // 会重复释放同一块内存，因为obj中含有指向arr动态内存的指针

    // Json类类型向基本数据类型的转换
    // bool b = v2;
    // int i = v3;
    // double d = v4;
    // string s = v5;

    cout << "hello hjm" << endl;
}

void test_parser() {
    // 数字的解析
    const string &str = "-1.23";
    Json v;
    v.parse(str);   // parse函数，将字符串解析成Json类

    // 字符串的解析   还有些问题：转义不太清楚
    const string &st = "\"hello\\u01E0 world\\b\"";
    cout << st << endl;
    Json s1;
    s1.parse(st);   // 所以s1中存的字符串有哪些字符？？
    cout << s1.str() << " " << "123" << endl;

    // 数组的解析
    const string &arr = "[\"a\", 1, 2, 3, false, true, null, 1.23]";    // arr是Json中以字符串的形式表示的数组？
    Json s2;
    s2.parse(arr);
    cout << s2.str() << endl;

    // 对象的解析
    const string &obj = "{\"a\":1, \"b\":2, \"c\":3}";
    Json s3;
    s3.parse(obj);
    cout << s3.str() << endl;
}

void test() {
    ifstream fin("./test_json.json");
    stringstream ss;
    ss << fin.rdbuf();
    const string &str = ss.str();

    Json v;
    v.parse(str);
    int code = v["code"];
    cout << code << endl;
    cout << v.str() << endl;
    // 尝试把结果重新输出成一个.json文件
    // 后续：1.测试运行速度
    //      2.尝试把结果重新输出成一个.json文件
    //      3.优化代码 RAII机制优化资源管理
    
}