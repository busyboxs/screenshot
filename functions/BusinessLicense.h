#pragma once
#include <string>
#include <iostream>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include "util.h"
#include "customVariables.h"

struct BusinessLicenseInfo
{
    uint64_t logID{};
    uint32_t resultNumber{};
    // WordsResult wordsResult;
    std::map<std::string, ResultPart> wordsResult{};

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

    void draw(cv::Mat& img)
    {
        for (auto& [name, res] : wordsResult)
        {
            res.draw(img);
        }
    }

};

class BusinessLicense
{
public:
    BusinessLicense();
    ~BusinessLicense();

    Json::Value request(std::string imgBase64, std::map<std::string, std::string>& options);

    void getResult(BusinessLicenseInfo& result);

private:
    Json::Value m_obj;
    std::string m_url;
    // file to save token key
    std::string m_filename;
};

void BusinessLicenseTest();
BusinessLicenseInfo BusinessLicenseDetect(std::string imgPath);