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
        {"Name", u8"����"},
        {"Relationship", u8"�������뻧����ϵ"},
        {"Sex", u8"�Ա�"},
        {"BirthAddress", u8"������"},
        {"Nation", u8"����"},
        {"Birthday", u8"����"},
        {"CardNo", u8"���֤��"},
        {"HouseholdNum", u8"����"},
        {"FormerName", u8"������"},
        {"Hometown", u8"����"},
        {"OtherAddress", u8"���У��أ�����סַ"},
        {"Belief", u8"�ڽ�����"},
        {"Height", u8"���"},
        {"BloodType", u8"Ѫ��"},
        {"Education", u8"�Ļ��̶�"},
        {"MaritalStatus", u8"����״��"},
        {"VeteranStatus", u8"����״��"},
        {"WorkAddress", u8"������"},
        {"Career", u8"ְҵ"},
        {"WWToCity", u8"��ʱ�ɺε�Ǩ������(��)"},
        {"WWHere", u8"��ʱ�ɺε�Ǩ����ַ"},
        {"Date", u8"�Ǽ�����"}
    };
};

void HouseholdTest();
HouseholdInfo HouseholdDetect(std::string imgPath);
