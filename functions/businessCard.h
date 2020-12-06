#pragma once
#include <string>
#include <iostream>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include "util.h"
#include "customVariables.h"

struct BusinessCardInfo
{
    uint64_t logID{};
    uint32_t resultNumber{};
    std::map<std::string, std::string> wordsResult{};

    void print()
    {
        std::cout << "log id: " << logID << '\n';
        std::cout << "words result number: " << resultNumber << '\n';

        for (auto& [name, res] : wordsResult)
        {
            std::cout << UTF8ToGB(name.c_str()) << ": " << UTF8ToGB(res.c_str()) << '\n';
        }
    }
};

class BusinessCard
{
public:
    BusinessCard();
    ~BusinessCard();

    Json::Value request(std::string imgBase64, std::map<std::string, std::string>& options);

    void getResult(BusinessCardInfo& result);

private:
    Json::Value m_obj;
    std::string m_url;
    // file to save token key
    std::string m_filename;

    inline static std::map<std::string, std::string> resultName
    {
        {"ADDR", u8"��ַ"},
        {"COMPANY", u8"��˾"},
        {"EMAIL", u8"����"},
        {"FAX", u8"����"},
        {"MOBILE", u8"�ֻ���"},
        {"NAME", u8"����"},
        {"PC", u8"�ʱ�"},
        {"TEL", u8"�绰"},
        {"TITLE", u8"ְλ"},
        {"URL", u8"��ַ"},
    };
};

void BusinessCardTest();
BusinessCardInfo BusinessCardDetect(std::string imgPath);
