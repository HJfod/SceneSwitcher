#include "Switcher.hpp"

SwitchTo g_eLastSwitch = kSwitchToMainMenu;
SwitchTo g_eLastSwitch2 = kSwitchToMainMenu;
GJGameLevel* g_pLastLevel = nullptr;
bool g_bLastLevelWasEdit = false;

constexpr const char* SwitchToToString(SwitchTo to) {
    switch (to) {
        case kSwitchToMainMenu: return "Main Menu";
        case kSwitchToIconKit: return "Icon Kit";
        case kSwitchToOfficialLevels: return "Official Levels";
        case kSwitchToMyLevels: return "My Levels";
        case kSwitchToOnlineLevels: return "Online Levels";
        case kSwitchToSearch: return "Search";
        case kSwitchToLast: return "Last";
        case kSwitchToLastLevel: return "Last Level";
        default: return "Unknown";
    }
}

constexpr const char* SwitchToToSprName(SwitchTo to) {
    switch (to) {
        case kSwitchToMainMenu: return "GJ_playBtn_001.png";
        case kSwitchToIconKit: return "GJ_garageBtn_001.png";
        case kSwitchToOfficialLevels: return "GJ_playBtn2_001.png";
        case kSwitchToMyLevels: return "GJ_createBtn_001.png";
        case kSwitchToOnlineLevels: return "GJ_creatorBtn_001.png";
        case kSwitchToSearch: return "GJ_searchBtn_001.png";
        default: return nullptr;
    }
}

const char* diffToSprName(GJGameLevel* lvl) {
    auto diff = lvl->getAverageDifficulty();
    if (diff == kGJDifficultyAuto) {
        return "difficulty_auto_btn_001.png";
    }
    if (lvl->m_nDemon) {
        return CCString::createWithFormat(
            "difficulty_%02d_btn2_001.png", 
            lvl->m_nDemonDifficulty
        )->getCString();
    }
    return CCString::createWithFormat(
        "difficulty_%02d_btn_001.png", diff
    )->getCString();
}

void setLastViewedLevel(GJGameLevel* lvl, bool b) {
    if (g_pLastLevel) {
        g_pLastLevel->release();
    }
    g_pLastLevel = lvl;
    g_pLastLevel->retain();
    g_bLastLevelWasEdit = b;
}


constexpr const int SWITCHER_TAG = 0x517E5;

bool Switch::init(const char* text, const char* sprName, SwitchTo to) {
    if (!CCNode::init())
        return false;
    
    this->m_eTo = to;

    this->setContentSize({ 80.f, 80.f });

    if (!text) {
        text = SwitchToToString(to);
    }

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->setPosition(this->getContentSize().width / 2, 10.f);
    label->limitLabelWidth(this->getContentSize().width - 5.f, .5f, .1f);
    this->addChild(label);

    if (!sprName) {
        sprName = SwitchToToSprName(to);
    }

    if (sprName) {
        auto spr = CCSprite::createWithSpriteFrameName(sprName);
        spr->setPosition({
            this->getContentSize().width / 2,
            this->getContentSize().height / 2 + 5.f
        });
        limitNodeSize(spr, this->getContentSize() / 1.5f, 1.f, .1f);
        this->addChild(spr);
    }

    return true;
}

void Switch::draw() {
    if (this->m_bHovered) {
        ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1, 1, 1, .2f });
    }

    CCNode::draw();
}

void Switch::updateHover(CCPoint const& pos) {
    this->m_bHovered = nodeIsHovered(this, pos - this->getContentSize() / 2);
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
        auto p = pos + CCDirector::sharedDirector()->getWinSize() / 2 - ret->getScaledContentSize() / 2;
        ret->setPosition(p);
        t->addChild(ret);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool Switcher::init() {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 230 }))
        return false;

    this->setTag(SWITCHER_TAG);
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto titleLabel = CCLabelBMFont::create("Go To Layer", "bigFont.fnt");
    titleLabel->setPosition(winSize.width / 2, winSize.height - 40.f);
    titleLabel->setScale(.6f);
    this->addChild(titleLabel);

    auto str = CCString::createWithFormat("Last Scene (%s)", SwitchToToString(g_eLastSwitch));

    auto lastLevel = g_pLastLevel ? 
        CCString::createWithFormat("Last Level (%s)", g_pLastLevel->m_sLevelName.c_str())->getCString() :
        "Last Level (None)";
    auto diffSpr = g_pLastLevel ?
        diffToSprName(g_pLastLevel) :
        "difficulty_00_btn_001.png";

    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToMainMenu, { -150.f, 50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToOfficialLevels, { -50.f, 50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToMyLevels, { 50.f, 50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToOnlineLevels, { 150.f, 50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToIconKit, { -150.f, -50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToSearch, { -50.f, -50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(lastLevel, diffSpr, kSwitchToLastLevel, { 50.f, -50.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(str->getCString(), SwitchToToSprName(g_eLastSwitch), kSwitchToLast, { 150.f, -50.f }, this)
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
    if (scene->getObjType() == kCCObjectTypePlayLayer ||
        scene->getObjType() == kCCObjectTypeLevelEditorLayer)
        return;

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
        auto targetScene = target->getWhere();
        if (targetScene == kSwitchToLast) {
            targetScene = g_eLastSwitch;
        } else if (targetScene != kSwitchToLastLevel) {
            g_eLastSwitch = g_eLastSwitch2;
            g_eLastSwitch2 = targetScene;
        }
        switch (targetScene) {
            case kSwitchToMainMenu:
                scene->addChild(MenuLayer::create());
                break;
                
            case kSwitchToIconKit:
                scene->addChild(GJGarageLayer::create());
                break;
                
            case kSwitchToMyLevels:
                scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(kGJSearchTypeMyLevels)));
                break;
                
            case kSwitchToSearch:
                scene->addChild(LevelSearchLayer::create());
                break;
                
            case kSwitchToOnlineLevels:
                scene->addChild(CreatorLayer::create());
                break;
                
            case kSwitchToOfficialLevels:
                scene->addChild(LevelSelectLayer::create(0));
                break;
            
            case kSwitchToLastLevel:
                if (g_pLastLevel) {
                    if (g_bLastLevelWasEdit) {
                        scene->addChild(EditLevelLayer::create(g_pLastLevel));
                    } else {
                        scene->addChild(LevelInfoLayer::create(g_pLastLevel));
                    }
                } else {
                    this->removeFromParent();
                    return;
                }
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
