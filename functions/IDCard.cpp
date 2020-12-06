#include "IDCard.h"

IDCard::IDCard()
{
    m_url = "https://aip.baidubce.com/rest/2.0/ocr/v1/idcard";
    m_filename = "tokenKey";
}

IDCard::~IDCard()
{
}

Json::Value IDCard::request(std::string imgBase64, std::map<std::string, std::string>& options)
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

void IDCard::getResult(IDCardInfo& result)
{
    if (m_obj.get("error_code", "null"))
    {
        result.wordsResult["error_code"].words = m_obj.get("error_code", "null").asString();
        result.wordsResult["error_msg"].words = m_obj.get("error_msg", "null").asString();
        return;
    }

    result.logID = m_obj["log_id"].asUInt64();
    result.direction = m_obj["direction"].asInt();
    result.imageStatus = m_obj["image_status"].asString();
    result.editTool = m_obj["edit_tool"].asString();
    result.photo = m_obj["photo"].asString();
    getLocation(m_obj["photo_location"], result.photoLocation);
    result.resultNumber = m_obj["words_result_num"].asInt();

    Json::Value::Members keys = m_obj["words_result"].getMemberNames();

    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        ResultPart resultPart;
        getResultPart(m_obj["words_result"][*it], resultPart);
        result.wordsResult[*it] = resultPart;
    }
}

void IDCardTest()
{
    std::string img_file = "./images/b.jpg";
    std::string out;
    readImageFile(img_file.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;
    options["id_card_side"] = "front";
    //options["detect_direction"] = "false";
    //options["detect_risk"] = "false";
    options["detect_photo"] = "true";
    //options["detect_rectify"] = "false";

    Json::Value obj;
    IDCard idCardObj;
    IDCardInfo result;
    obj = idCardObj.request(img_base64, options);

    idCardObj.getResult(result);
    result.print();

    cv::Mat img = cv::imread("./images/b.jpg");
    result.draw(img);
    cv::namedWindow("idcard", cv::WINDOW_NORMAL);
    cv::imshow("idcard", img);
    cv::imwrite("./images/idcard_result.jpg", img);
    cv::waitKey();
}

IDCardInfo IDCardDetect(std::string imgPath, bool front)
{
    std::string out;
    readImageFile(imgPath.c_str(), out);
    std::string img_base64 = base64_encode(out.c_str(), (int)out.size());

    // set options
    std::map<std::string, std::string> options;

    options["id_card_side"] = front ? "front" : "back";
    options["detect_photo"] = "true";

    Json::Value obj;
    IDCard idCardObj;
    IDCardInfo result;
    obj = idCardObj.request(img_base64, options);

    idCardObj.getResult(result);

    return result;
}
