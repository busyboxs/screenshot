#pragma once
#include "util.h"
#include <iostream>

struct Location
{
    uint32_t top{};
    uint32_t left{};
    uint32_t width{};
    uint32_t height{};

    Location& operator= (const Location& location)
    {
        top = location.top;
        left = location.left;
        width = location.width;
        height = location.height;
        return *this;
    }

    void print()
    {
        std::cout << "[" << top << ", " << left << ", " << width << ", " << height << "]\n";
    }

    void draw(cv::Mat& img)
    {
        cv::rectangle(img, cv::Point(left, top), cv::Point(left + width, top + height), cv::Scalar(255, 255, 0), 2);
    }
};

struct ResultPart
{
    Location     location{};
    std::string  words{};

    void print()
    {
        location.print();
        std::cout << "\t\twords: " << words << '\n';
    }

    void draw(cv::Mat& img)
    {
        location.draw(img);
    }
};

struct WordsOnlyPart
{
    std::string words{};

    void print()
    {
        std::cout << "\t\twords: " << words << '\n';
    }
};