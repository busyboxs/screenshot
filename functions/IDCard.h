#pragma once
#include <string>
#include <iostream>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include "util.h"
#include "customVariables.h"

// https://cloud.baidu.com/doc/OCR/s/rk3h7xzck
struct IDCardInfo
{
    uint64_t                          logID{};
    int32_t                           direction{};
    uint32_t                          resultNumber{};
    std::string                       imageStatus{};
    std::string                       riskType{};
    std::string                       editTool{};
    std::string                       photo{};
    Location                          photoLocation{};
    std::map<std::string, ResultPart> wordsResult{};

    void print()
    {
        std::cout << "log_id: " << logID << '\n';
        std::cout << "direction: " << direction << '\n';
        std::cout << "imageStatus: " << imageStatus << '\n';
        std::cout << "riskType: " << riskType << '\n';
        std::cout << "editTool: " << editTool << '\n';
        std::cout << "photo: " << photo << '\n';
        std::cout << "photo location: ";
        photoLocation.print();
        std::cout << "result number: " << resultNumber << '\n';

        for (auto& [name, res] : wordsResult)
        {
            std::cout << "\t" << UTF8ToGB(name.c_str()) << ": ";
            res.print();
        }

    }

    void draw(cv::Mat& img)
    {
        photoLocation.draw(img);
        for (auto& [name, res] : wordsResult)
        {
            res.draw(img);
        }
    }
};

class IDCard
{
public:
    IDCard();
    ~IDCard();

    Json::Value request(std::string imgBase64, std::map<std::string, std::string>& options);

    void getResult(IDCardInfo& result);

private:
    Json::Value m_obj;
    std::string m_url;
    // file to save token key
    std::string m_filename;
};

void IDCardTest();
IDCardInfo IDCardDetect(std::string imgPath, bool front);
