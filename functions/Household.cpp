#include "Household.h"

Household::Household()
{
    m_url = "https://aip.baidubce.com/rest/2.0/ocr/v1/household_register";
    m_filename = "tokenKey";
}

Household::~Household()
{
}

Json::Value Household::request(std::string imgBase64, std::map<std::string, std::string>& options)
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

void Household::getResult(HouseholdInfo& result)
{
    if (m_obj.get("error_code", "null"))
    {
        result.wordsResult["error_code"].words = m_obj.get("error_code", "null").asString();
        result.wordsResult["error_msg"].words = m_obj.get("error_msg", "null").asString();
        return;
    }

    result.logID = m_obj["log_id"].asUInt64();
    result.resultNumber = m_obj["words_result_num"].asUInt();
    Json::Value::Members keys = m_obj["words_result"].getMemberNames();

    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        WordsOnlyPart resultPart;
        getWordsOnlyPart(m_obj["words_result"][*it], resultPart);
        result.wordsResult[resultName[*it].c_str()] = resultPart;
    }
}

void HouseholdTest()
{
    std::string img_file = "./images/household_test.png";
    std::string out;
    readImageFile(img_file.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;

    Json::Value obj;
    Household householdObj;
    HouseholdInfo result;
    obj = householdObj.request(img_base64, options);

    householdObj.getResult(result);
    result.print();
}

HouseholdInfo HouseholdDetect(std::string imgPath)
{
    std::string out;
    readImageFile(imgPath.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;

    Json::Value obj;
    Household householdObj;
    HouseholdInfo result;
    obj = householdObj.request(img_base64, options);

    householdObj.getResult(result);

    return result;
}
