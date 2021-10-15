#pragma once

#include <GDMake.h>

using namespace cocos2d;
using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;

static CCPoint getMousePos() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
    mpos.y = winSizePx.size.height - mpos.y;
    mpos.x *= ratio_w;
    mpos.y *= ratio_h;

    return mpos;
}

static bool nodeIsHovered(CCNode* node, CCPoint const& gpos) {
    auto pos = node->getParent()->convertToWorldSpace(node->getPosition());
    auto size = node->getScaledContentSize();

    auto rect = CCRect {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };

    return rect.containsPoint(gpos);
}

static bool operator!=(CCPoint const& p1, CCPoint const& p2) {
    return p1.x != p2.x || p1.y != p2.y;
}

static void limitNodeSize(CCNode* spr, CCSize const& size, float def, float min) {
    spr->setScale(1.f);
    auto [cwidth, cheight] = spr->getContentSize();

    float scale = def;
    if (size.height && size.height < cheight) {
        scale = size.height / cheight;
    }
    if (size.width && size.width < cwidth) {
        if (size.width / cwidth < scale)
            scale = size.width / cwidth;
    }
    if (def && def < scale) {
        scale = def;
    }
    if (min && scale < min) {
        scale = min;
    }
    spr->setScale(scale);
}

static std::vector<std::string> stringSplit(std::string const& orig, std::string const& split) {
    std::vector<std::string> res;

    if (orig.size()) {
        auto s = orig;

        size_t pos = 0;

        while ((pos = s.find(split)) != std::string::npos) {
            res.push_back(s.substr(0, pos));
            s.erase(0, pos + split.length());
        }

        res.push_back(s);
    }

    return res;
}

static std::string stringToLower(std::string const& orig) {
    auto res = orig;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c){ return std::tolower(c); });
    return res;
}

static std::string stringReplace(std::string const& orig, std::string const& subs, std::string const& reps) {
    std::string::size_type n = 0;
    std::string res = orig;

    while ( ( n = res.find( subs, n ) ) != std::string::npos ) {
        res.replace( n, subs.size(), reps );
        n += reps.size();
    }

    return res;
}

static bool stringContains(std::string const& orig, std::string const& subs) {
    return orig.find(subs) != std::string::npos;
}

