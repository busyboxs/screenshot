#include "util.h"

// callback function for curl
size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* str = dynamic_cast<std::string*>((std::string*)userdata);
    str->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

// get access token from server by get method
std::string getTokenKey() {
    std::string url = "https://aip.baidubce.com/oauth/2.0/token";
    std::string apikey = "nXb3wUW1xSPav1vMN3BMetAl";
    std::string secritkey = "DSlxiE0W1I3Sp3hvhCEcCg7IWE0LBSkd";
    std::map<std::string, std::string> params;
    std::string response;

    params["grant_type"] = "client_credentials";
    params["client_id"] = apikey;
    params["client_secret"] = secritkey;

    // append url with parameters
    for (auto it = params.begin(); it != params.end(); ++it) {
        url += (it == params.begin() ? "?" : "&") + it->first + "=" + it->second;
    }

    CURL* curl = curl_easy_init();

    struct curl_slist* slist = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);  // set callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // set var to receive return info from callback function
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, false);

    int status_code = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(slist);

    Json::Value obj;
    if (status_code != CURLcode::CURLE_OK) {
        obj["curl_error_code"] = status_code;
        return obj.toStyledString();
    }

    // parse json string
    JSONCPP_STRING error;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    reader->parse(response.data(), response.data() + response.size(), &obj, &error);
    std::string access_token = obj["access_token"].asString();

    return access_token;
}

// read image file [https://stackoverflow.com/questions/9612121/how-to-read-image-files-and-store-it-in-memorystdstring-in-c]
int readImageFile(const char* filename, std::string& out) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::ostringstream oss;
        oss << in.rdbuf();
        out.assign(oss.str());
        return 0;
    }
    else {
        std::cerr << "Can't open image!" << std::endl;
        return -1;
    }
}

void imgToBase64(std::string& imgFile, std::string& imgBase64) {
    // read image and encode to base64
    std::string out;
    readImageFile(imgFile.c_str(), out);
    imgBase64 = base64_encode(out.c_str(), (int)out.size());
}

int writeFile(const std::string& fileString, const std::string& str) {
    std::ofstream out(fileString, std::ios::binary);
    if (out.is_open()) {
        out << str;
        out.close();
    }

    return 0;
}

int readFile(const std::string& fileString, std::string& str) {
    std::ifstream in(fileString);
    if (!in.is_open()) {
        str = "";
        return -1;
    }

    char buffer[256];
    while (!in.eof()) {
        in.getline(buffer, sizeof(buffer));
    }

    str = buffer;
    return 0;
}


std::string UTF8ToGB(const char* str)
{
    std::string result;
    WCHAR* strSrc;
    LPSTR szRes;

    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new CHAR[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[]strSrc;
    delete[]szRes;
    return result;
}

void mergeHttpPostBody(std::string& body, std::string imgBase64, std::map<std::string, std::string>& options) {
    body = "image=" + std::string(curl_escape(imgBase64.c_str(), int(imgBase64.size()))) + "&";
    // append body with options
    for (auto it = options.begin(); it != options.end(); ++it) {
        body += std::string(curl_escape(it->first.c_str(), (int)it->first.size()))
            + "=" + std::string(curl_escape(it->second.c_str(), (int)it->second.size())) + "&";
    }
}

// first get token from file (if exist), then add token to url
void getHttpPostUrl(std::string& url, std::string& filename, std::string& token) {
    // if token file is not exist, a new one should be create
    if (readFile(filename, token) < 0) {
        token = getTokenKey();
        writeFile(filename, token);
    }

    url = url + "?access_token=" + token;
}

int httpPostRequest(std::string& url, std::string& body, std::string& response) {
    struct curl_slist* slist = NULL;

    CURL* curl = curl_easy_init();

    // set headers, actually this is used by default
    std::string headers = "Content-Type:application/x-www-form-urlencoded";
    slist = curl_slist_append(slist, headers.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_POST, true);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, false);

    int status_code = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(slist);

    return status_code;
}

void generateJson(std::string& response, Json::Value& obj) {
    // parse json string
    JSONCPP_STRING error;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    reader->parse(response.data(), response.data() + response.size(), &obj, &error);
}

void checkErrorWithExit(Json::Value& obj)
{
    if (obj.get("error_code", "null")) {
        std::cerr << obj.get("error_code", "null") << " : " << obj.get("error_msg", "null") << std::endl;
        system("pause");
        exit(EXIT_FAILURE);
    }
}

void getLocation(Json::Value& loc, Location& location)
{
    location.left = loc["left"].asInt();
    location.top = loc["top"].asInt();
    location.width = loc["width"].asInt();
    location.height = loc["height"].asInt();
}

void getResultPart(Json::Value& res, ResultPart& resultPart)
{
    getLocation(res["location"], resultPart.location);
    resultPart.words = res["words"].asString().c_str();
}

void getWordsOnlyPart(Json::Value& res, WordsOnlyPart& wordsOnlyPart)
{
    wordsOnlyPart.words = res["words"].asString().c_str();
}
