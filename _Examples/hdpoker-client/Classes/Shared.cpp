#include "Shared.h"
#include "SimpleAudioEngine.h"
#include <sstream>
#include <chrono>

using namespace cocos2d;
using namespace std::chrono;

const char *const UniSansBold = "fonts/Uni-Sans-Bold.ttf";
const char *const UniSansRegular = "fonts/Uni-Sans-Regular.ttf";

// Server is not giving us this data yet.
static const char *TierName[] = { "Bronze", "Silver", "Gold", "Diamond", "Master" };
static const int TierBaseEloPoints[] = { 400, 800, 1600, 2400, 3800 };

std::string shortStyleNumber(int64_t num) {
    std::ostringstream oss;
    oss << num;
    auto data = oss.str();
    
    std::string append = "";
    int cull = 0;
    if (data.length() > 9) {
        append = "B";
        cull = 8;
    }
    else if (data.length() > 6) {
        append = "M";
        cull = 5;
    }
    else if (data.length() > 3) {
        append = "K";
        cull = 2;
    }
    else {
        return data;
    }
    
    std::string ret = data.substr(0, data.length() - cull);
    
    int num_commas = (ret.length() - 2) / 3;
    
    for (int i = 0; i < num_commas; ++i) {
        ret.insert(ret.length() - ((i + 1) * 3) - i - 1, ",");
    }
    
    ret.insert(ret.length() - 1, ".");
    ret.append(append);
    // CCLOG("Chopping %s to %s", data.c_str(), ret.c_str());
    return ret;
}

std::string toUpper(const char* str) {
    auto upper = std::string(str);
    int len = static_cast<int>(upper.length());
    for(auto i = 0; i < len; ++i) {
        upper[i] = toupper(upper[i]);
    }
    return upper;
}

std::chrono::milliseconds::rep getTimeInMilliseconds() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void coverFill(Node *node, Size containSize) {
    node->setAnchorPoint(Vec2(.5, .5));
    auto size = node->getContentSize();
    auto scale = std::max(containSize.height / size.height, containSize.width / size.width);
    node->setScale(scale);
//    node->setPosition(Vec2(containSize.width / 2, containSize.height / 2));
}

float containScale(Size size, Size containerSize) {
    return std::min(containerSize.height / size.height, containerSize.width / size.width);
}

Size containSize(Size size, Size containerSize) {
    auto scale = containScale(size, containerSize);
    return Size(scale * size.width, scale * size.height);
}

void playNotImplemented() {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((std::string("sounds/debug/") + std::to_string(rand() % 2) + std::string(".wav")).c_str());
}

const char *getTierName(int tier) {
	if (tier < 0 || tier > 4) return "";
	return TierName[tier];
}

int getTierFromElo(const int eloPoints) {
	int tier = 4;
	for (; tier > 0; tier--) {
		if (eloPoints >= TierBaseEloPoints[tier])	{
			break;
		}
	}
	return tier;
}

int getBaseEloPoints(const int tier) {
	if (tier < 0 || tier > 4) return 0;
	return TierBaseEloPoints[tier];
}

