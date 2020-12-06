#pragma once
#include <string>
#include <iostream>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include "util.h"
#include "customVariables.h"

struct HouseholdInfo
{
    uint64_t logID{};
    uint32_t resultNumber{};
    std::map<std::string, WordsOnlyPart> wordsResult{};

    void print()
    {
        std::cout << "log id: " << logID << '\n';
        std::cout << "words result number: " << resultNumber << '\n';

        for (auto& [name, res] : wordsResult)
        {
            std::cout << "\t" << UTF8ToGB(name.c_str()) << ": ";
            res.print();
        }
    }
};

class Household
{
public:
    Household();
    ~Household();

    Json::Value request(std::string imgBase64, std::map<std::string, std::string>& options);

    void getResult(HouseholdInfo& result);

private:
    Json::Value m_obj;
    std::string m_url;
    // file to save token key
    std::string m_filename;

    inline static std::map<std::string, std::string> resultName
    {
        {"Name", u8"姓名"},
        {"Relationship", u8"户主或与户主关系"},
        {"Sex", u8"性别"},
        {"BirthAddress", u8"出生地"},
        {"Nation", u8"民族"},
        {"Birthday", u8"生日"},
        {"CardNo", u8"身份证号"},
        {"HouseholdNum", u8"户号"},
        {"FormerName", u8"曾用名"},
        {"Hometown", u8"籍贯"},
        {"OtherAddress", u8"本市（县）其他住址"},
        {"Belief", u8"宗教信仰"},
        {"Height", u8"身高"},
        {"BloodType", u8"血型"},
        {"Education", u8"文化程度"},
        {"MaritalStatus", u8"婚姻状况"},
        {"VeteranStatus", u8"兵役状况"},
        {"WorkAddress", u8"服务处所"},
        {"Career", u8"职业"},
        {"WWToCity", u8"何时由何地迁来本市(县)"},
        {"WWHere", u8"何时由何地迁往本址"},
        {"Date", u8"登记日期"}
    };
};

void HouseholdTest();
HouseholdInfo HouseholdDetect(std::string imgPath);
