#pragma once

#include <string>
#include "cocos2d.h"

std::string shortStyleNumber(int64_t num);

const int64_t chipDenominations[] = {
    100000000000,
    25000000000,
    5000000000,
    1000000000,
    500000000,
    100000000,
    25000000,
    5000000,
    1000000,
    500000,
    100000,
    25000,
    5000,
    1000,
    500,
    100,
    25,
    5,
    1
};
const int chipDenominationsSize = sizeof(chipDenominations) / sizeof(*chipDenominations);
const int chipStackSpacing = 4;
const float chipScale = 1;
const int chipWidth = 33;

extern const char *const UniSansBold;
extern const char *const UniSansRegular;

template <typename T> std::string numberWithCommas(T number) {
    std::string numWithCommas = std::to_string(number);
    int insertPosition = numWithCommas.length() - 3;
    while (insertPosition > 0) {
        numWithCommas.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return numWithCommas;
}

std::chrono::milliseconds::rep getTimeInMilliseconds();

void coverFill(cocos2d::Node *node, cocos2d::Size containSize);
float containScale(cocos2d::Size size, cocos2d::Size containerSize);
cocos2d::Size containSize(cocos2d::Size objectSize, cocos2d::Size containerSize);

void playNotImplemented();