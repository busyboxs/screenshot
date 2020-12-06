#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include "util.h"
#include "customVariables.h"

struct BankCardInfo
{
    uint64_t logID{};
    uint32_t direction{};
    std::map<std::string, std::string> resultMap{};

    void print()
    {
        std::cout << "log id: " << logID << '\n';
        std::cout << "direction: " << direction << '\n';

        for (auto& [name, res] : resultMap)
        {
            std::cout << UTF8ToGB(name.c_str()) << ": " << UTF8ToGB(res.c_str()) << '\n';
        }
    }
};

class BankCard
{
public:
    BankCard();
    ~BankCard();

    Json::Value request(std::string imgBase64, std::map<std::string, std::string>& options);

    void getResult(BankCardInfo& result);

private:
    Json::Value m_obj;
    std::string m_url;
    // file to save token key
    std::string m_filename;

    inline static std::vector<std::string> cardTypes{
        u8"不能识别",
        u8"借记卡",
        u8"贷记卡",
        u8"准贷记卡",
        u8"预付费卡"
    };

    inline static std::map<std::string, std::string> resultName{
        {"bank_card_number", u8"银行卡卡号"},
        {"valid_date", u8"有效期"},
        {"bank_card_type", u8"银行卡类型"},
        {"bank_name", u8"银行名称"}
    };
};

void BankCardTest();
BankCardInfo BandCardDetect(std::string imgPath);
