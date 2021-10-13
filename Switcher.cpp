#include "Switcher.hpp"

class LevelSelectLayer : public CCLayer {
    public:
        static LevelSelectLayer* create(int lvl) {
            return reinterpret_cast<LevelSelectLayer*(__fastcall*)(int)>(
                base + 0x185500
            )(lvl);
        }
};

class CreatorLayer : public CCLayer {
    public:
        static CreatorLayer* create() {
            return reinterpret_cast<CreatorLayer*(__fastcall*)()>(
                base + 0x4dda0
            )();
        }
};

constexpr const int SWITCHER_TAG = 0x517E5;

bool Switch::init(const char* text, const char* sprName, SwitchTo to) {
    if (!CCNode::init())
        return false;
    
    this->m_eTo = to;

    this->setContentSize({ 60.f, 60.f });

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->setPosition(this->getContentSize().width / 2, 10.f);
    label->limitLabelWidth(this->getContentSize().width, .5f, .1f);
    this->addChild(label);

    if (sprName) {
        auto spr = CCSprite::createWithSpriteFrameName(sprName);
        spr->setPosition({
            this->getContentSize().width / 2,
            this->getContentSize().height / 2 + 5.f
        });
        this->addChild(spr);
    }

    return true;
}

void Switch::draw() {
    if (this->m_bHovered) {
        ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1, 1, 1, .2f });
    }

    CCNode::draw();
}

void Switch::updateHover(CCPoint const& pos) {
    this->m_bHovered = nodeIsHovered(this, pos);
}

SwitchTo Switch::getWhere() const {
    return this->m_eTo;
}

bool Switch::isHovered() const {
    return this->m_bHovered;
}

Switch* Switch::create(const char* text, const char* spr, SwitchTo to, CCPoint const& pos, CCNode* t) {
    auto ret = new Switch;

    if (ret && ret->init(text, spr, to)) {
        ret->autorelease();
        auto p = pos + CCDirector::sharedDirector()->getWinSize() / 2;
        ret->setPosition(p);
        t->addChild(ret);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool Switcher::init() {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 150 }))
        return false;
    
    this->setTag(SWITCHER_TAG);

    this->m_vSwitches.push_back(
        Switch::create("Main Menu", nullptr, kSwitchToMainMenu, { -150.f, 40.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create("Official Levels", nullptr, kSwitchToOfficialLevels, { -70.f, 40.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create("My Levels", nullptr, kSwitchToMyLevels, { 70.f, 40.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create("Online Levels", nullptr, kSwitchToOnlineLevels, { 150.f, 40.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create("Icon Kit", nullptr, kSwitchToIconKit, { -150.f, -40.f }, this)
    );
    
    return true;
}

void Switcher::visit() {
    if (this->isVisible()) {
        auto pos = getMousePos();
        for (auto s : this->m_vSwitches) {
            s->updateHover(pos);
        }
    }
    CCLayer::visit();
}

void Switcher::show() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene->getChildByTag(SWITCHER_TAG)) {
        scene->getChildByTag(SWITCHER_TAG)->setVisible(true);
    } else {
        scene->addChild(Switcher::create());
    }
}

void Switcher::go() {
    Switch* target = nullptr;
    auto pos = getMousePos();
    for (auto s : this->m_vSwitches) {
        if (s->isHovered()) {
            target = s;
            break;
        }
    }
    if (target) {
        auto scene = CCScene::create();
        switch (target->getWhere()) {
            case kSwitchToMainMenu:
                scene->addChild(MenuLayer::create());
                break;
                
            case kSwitchToIconKit:
                scene->addChild(GJGarageLayer::create());
                break;
                
            case kSwitchToMyLevels:
                scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(kGJSearchTypeMyLevels)));
                break;
                
            case kSwitchToOnlineLevels:
                scene->addChild(CreatorLayer::create());
                break;
                
            case kSwitchToOfficialLevels:
                scene->addChild(LevelSelectLayer::create(0));
                break;
        }
        CCDirector::sharedDirector()->replaceScene(scene);
    }
    this->removeFromParent();
}

void Switcher::goTo() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene->getChildByTag(SWITCHER_TAG)) {
        as<Switcher*>(scene->getChildByTag(SWITCHER_TAG))->go();
    }
}

Switcher* Switcher::create() {
    auto ret = new Switcher;

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
