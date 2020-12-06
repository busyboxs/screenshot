#pragma once
#include <curl/curl.h>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "base64.h"
#include "customVariables.h"

size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata);
std::string getTokenKey();
int readImageFile(const char* filename, std::string& out);
void imgToBase64(std::string& imgFile, std::string& imgBase64);
int writeFile(const std::string& fileString, const std::string& str);
int readFile(const std::string& fileString, std::string& str);
std::string UTF8ToGB(const char* str);
void mergeHttpPostBody(std::string& body, std::string imgBase64, std::map<std::string, std::string>& options);
void getHttpPostUrl(std::string& url, std::string& filename, std::string& token);
int httpPostRequest(std::string& url, std::string& body, std::string& response);
void generateJson(std::string& response, Json::Value& obj);
void checkErrorWithExit(Json::Value& obj);

void getLocation(Json::Value& loc, Location& location);
void getResultPart(Json::Value& res, ResultPart& resultPart);
void getWordsOnlyPart(Json::Value& res, WordsOnlyPart& wordsOnlyPart);