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

