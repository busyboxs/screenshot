#include "bankcard.h"

BankCard::BankCard()
{
    m_url = "https://aip.baidubce.com/rest/2.0/ocr/v1/bankcard";
    m_filename = "tokenKey";
}

BankCard::~BankCard()
{
}

Json::Value BankCard::request(std::string imgBase64, std::map<std::string, std::string>& options)
{
    std::string response;
    Json::Value obj;
    std::string token;

    // 1. get HTTP post body
    std::string body;
    mergeHttpPostBody(body, imgBase64, options);

    // 2. get HTTP url with access token
    std::string url = m_url;
    getHttpPostUrl(url, m_filename, token);

    // 3. post request, response store the result
    int status_code = httpPostRequest(url, body, response);
    if (status_code != CURLcode::CURLE_OK) {
        obj["curl_error_code"] = status_code;
        m_obj = obj;
        return obj; // TODO: maybe should exit 
    }

    // 4. make string to json object
    generateJson(response, obj);

    // if access token is invalid or expired, we will get a new one
    if (obj["error_code"].asInt() == 110 || obj["error_code"].asInt() == 111) {
        token = getTokenKey();
        writeFile(m_filename, token);
        return request(imgBase64, options);

    }

    m_obj = obj;

    //checkErrorWithExit(obj);

    return obj;
}

void BankCard::getResult(BankCardInfo& result)
{
    if (m_obj.get("error_code", "null"))
    {
        result.resultMap["error_code"] = m_obj.get("error_code", "null").asString();
        result.resultMap["error_msg"] = m_obj.get("error_msg", "null").asString();
        return;
    }

    result.logID = m_obj["log_id"].asUInt64();
    result.direction = m_obj["direction"].asUInt();
    Json::Value::Members keys = m_obj["result"].getMemberNames();

    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        if (auto type = *it; type == "bank_card_type")
        {
            result.resultMap[resultName[*it].c_str()] = cardTypes[m_obj["result"][*it].asInt()];
        }
        else
        {
            result.resultMap[resultName[*it].c_str()] = m_obj["result"][*it].asString();
        }
    }
}

void BankCardTest()
{
    std::string img_file = "./images/bandcard_test.jpg";
    std::string out;
    readImageFile(img_file.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;

    Json::Value obj;
    BankCard bandcardObj;
    BankCardInfo result;
    obj = bandcardObj.request(img_base64, options);

    bandcardObj.getResult(result);
    result.print();
}

BankCardInfo BandCardDetect(std::string imgPath)
{
    std::string out;
    readImageFile(imgPath.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;

    Json::Value obj;
    BankCard bandcardObj;
    BankCardInfo result;
    obj = bandcardObj.request(img_base64, options);

    bandcardObj.getResult(result);
    return result;
}
