#include "Switcher.hpp"

SwitchTo g_eLastSwitch = kSwitchToMainMenu;
SwitchTo g_eCurrentScene = kSwitchToMainMenu;
GJGameLevel* g_pLastLevel = nullptr;
LastLevelType g_eLastLevelWasEdit = kLastLevelTypeMain;

CCPoint g_obMousePos;
CCPoint g_obHighlightPos;

constexpr const char* SwitchToToString(SwitchTo to) {
    switch (to) {
        case kSwitchToMainMenu: return "Main Menu";
        case kSwitchToIconKit: return "Icon Kit";
        case kSwitchToOfficialLevels: return "Official Levels";
        case kSwitchToMyLevels: return "My Levels";
        case kSwitchToOnlineLevels: return "Online Levels";
        case kSwitchToSavedLevels: return "Saved Levels";
        case kSwitchToMapPacks: return "Map Packs";
        case kSwitchToSearch: return "Search";
        case kSwitchToTheChallenge: return "The Challenge";
        case kSwitchToDaily: return "Daily";
        case kSwitchToWeekly: return "Weekly";
        case kSwitchToLast: return "Last";
        case kSwitchToLastLevel: return "Last Level";
        case kSwitchToSettings: return "Settings";
        case kSwitchToProfile: return "Profile";
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
        case kSwitchToSavedLevels: return "GJ_savedBtn_001.png";
        case kSwitchToMapPacks: return "GJ_mapPacksBtn_001.png";
        case kSwitchToSettings: return "GJ_optionsBtn_001.png";
        case kSwitchToProfile: return "GJ_profileButton_001.png";
        case kSwitchToTheChallenge: return "difficulty_10_btn_001.png";
        case kSwitchToDaily: return "GJ_dailyBtn_001.png";
        case kSwitchToWeekly: return "GJ_weeklyBtn_001.png";
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

void setLastViewedLevel(GJGameLevel* lvl, LastLevelType b) {
    if (g_pLastLevel) {
        g_pLastLevel->release();
    }
    g_pLastLevel = lvl;
    g_pLastLevel->retain();
    g_eLastLevelWasEdit = b;
}


constexpr const int SWITCHER_TAG = 0x517E5;

bool Switch::init(const char* text, const char* sprName, SwitchTo to, float scale) {
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
        if (spr) {
            spr->setPosition({
                this->getContentSize().width / 2,
                this->getContentSize().height / 2 + 5.f
            });
            limitNodeSize(spr, this->getContentSize() / 1.5f, scale, .1f);
            this->addChild(spr);
        }
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

Switch* Switch::create(const char* text, const char* spr, SwitchTo to, CCPoint const& pos, CCNode* t, float scale) {
    auto ret = new Switch;

    if (ret && ret->init(text, spr, to, scale)) {
        ret->autorelease();
        auto p = pos + CCDirector::sharedDirector()->getWinSize() / 2 - ret->getScaledContentSize() / 2;
        ret->setPosition(p);
        t->addChild(ret);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


constexpr bool Switcher::isPopupSwitchType(SwitchTo to) {
    switch (to) {
        case kSwitchToSettings:
        case kSwitchToProfile:
        case kSwitchToDaily:
        case kSwitchToWeekly:
            return true;
    }
    return false;
}

bool Switcher::init() {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 230 }))
        return false;

    this->setTag(SWITCHER_TAG);
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto titleLabel = CCLabelBMFont::create("Go To Layer", "bigFont.fnt");
    titleLabel->setPosition(winSize.width / 2, winSize.height - 15.f);
    titleLabel->setScale(.4f);
    this->addChild(titleLabel);

    auto str = CCString::createWithFormat("Last Scene (%s)", SwitchToToString(g_eLastSwitch));

    auto lastLevel = g_pLastLevel ? 
        CCString::createWithFormat("Last Level (%s)", g_pLastLevel->m_sLevelName.c_str())->getCString() :
        "Last Level (None)";
    auto diffSpr = g_pLastLevel ?
        diffToSprName(g_pLastLevel) :
        "difficulty_00_btn_001.png";

    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToMainMenu, { -150.f, 100.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToMyLevels, { -50.f, 100.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToOnlineLevels, { 50.f, 100.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToIconKit, { 150.f, 100.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToSearch, { -150.f, 0.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToSavedLevels, { -50.f, 0.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToDaily, { 50.f, 0.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToWeekly, { 150.f, 0.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToSettings, { -150.f, -100.f }, this, .8f)
    );
    this->m_vSwitches.push_back(
        Switch::create(nullptr, nullptr, kSwitchToProfile, { -50.f, -100.f }, this, .8f)
    );
    this->m_vSwitches.push_back(
        Switch::create(lastLevel, diffSpr, kSwitchToLastLevel, { 50.f, -100.f }, this)
    );
    this->m_vSwitches.push_back(
        Switch::create(str->getCString(), SwitchToToSprName(g_eLastSwitch), kSwitchToLast, { 150.f, -100.f }, this)
    );
    
    return true;
}

void Switcher::visit() {
    if (this->isVisible()) {
        auto pos = getMousePos();
        if (pos != g_obMousePos) {
            g_obHighlightPos = pos;
            g_obMousePos = pos;
        }
        for (auto s : this->m_vSwitches) {
            s->updateHover(g_obHighlightPos);
        }
    }
    CCLayer::visit();
}

bool snapHighlight() {
    bool ret = false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (winSize.width / 2 - 190.f > g_obHighlightPos.x) {
        g_obHighlightPos.x = winSize.width / 2 - 150.f;
        ret = true;
    }
    if (winSize.width / 2 + 190.f < g_obHighlightPos.x) {
        g_obHighlightPos.x = winSize.width / 2 + 150.f;
        ret = true;
    }
    if (winSize.height / 2 - 140.f > g_obHighlightPos.y) {
        g_obHighlightPos.y = winSize.height / 2 - 100.f;
        ret = true;
    }
    if (winSize.height / 2 + 140.f < g_obHighlightPos.y) {
        g_obHighlightPos.y = winSize.height / 2 + 100.f;
        ret = true;
    }

    g_obHighlightPos.x = roundf(g_obHighlightPos.x / 100.f) * 100.f - 50.f;
    g_obHighlightPos.y = roundf(g_obHighlightPos.y / 100.f) * 100.f - 50.f;

    return ret;
}

bool Switcher::handle(enumKeyCodes key) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    switch (key) {
        case KEY_Left:
            if (!snapHighlight()) {
                g_obHighlightPos.x -= 100.f;
                if (winSize.width / 2 - 190.f > g_obHighlightPos.x)
                    g_obHighlightPos.x += 400.f;
            }
            return true;
        
        case KEY_Right:
            if (!snapHighlight()) {
                g_obHighlightPos.x += 100.f;
                if (winSize.width / 2 + 190.f < g_obHighlightPos.x)
                    g_obHighlightPos.x -= 400.f;
            }
            return true;
        
        case KEY_Up:
            if (!snapHighlight()) {
                g_obHighlightPos.y += 100.f;
                if (winSize.height / 2 + 140.f < g_obHighlightPos.y)
                    g_obHighlightPos.y -= 300.f;
            }
            return true;
        
        case KEY_Down:
            if (!snapHighlight()) {
                g_obHighlightPos.y -= 100.f;
                if (winSize.height / 2 - 140.f > g_obHighlightPos.y)
                    g_obHighlightPos.y += 300.f;
            }
            return true;
    }

    return false;
}

bool Switcher::handleKey(enumKeyCodes key) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene->getChildByTag(SWITCHER_TAG)) {
        return as<Switcher*>(scene->getChildByTag(SWITCHER_TAG))->handle(key);
    }
    return false;
}

void Switcher::show() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene->getObjType() == kCCObjectTypePlayLayer ||
        scene->getObjType() == kCCObjectTypeLevelEditorLayer)
        return;
    
    auto z = scene->getHighestChildZ();

    if (scene->getChildByTag(SWITCHER_TAG)) {
        scene->getChildByTag(SWITCHER_TAG)->setVisible(true);
        scene->getChildByTag(SWITCHER_TAG)->setZOrder(z + 0x99);
    } else {
        scene->addChild(Switcher::create());
        scene->getChildByTag(SWITCHER_TAG)->setZOrder(z + 0x99);
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
        auto targetScene = target->getWhere();
        if (isPopupSwitchType(targetScene)) {
            switch (targetScene) {
                case kSwitchToProfile: {
                    auto page = ProfilePage::create(
                        GJAccountManager::sharedState()->m_nPlayerAccountID, true
                    );
                    page->m_bNoElasticity = true;
                    page->show();
                } break;

                case kSwitchToSettings:
                    OptionsLayer::addToCurrentScene(true);
                    break;

                case kSwitchToDaily: {
                    auto page = DailyLevelPage::create(false);
                    page->m_bNoElasticity = true;
                    page->show();
                } break;

                case kSwitchToWeekly: {
                    auto page = DailyLevelPage::create(true);
                    page->m_bNoElasticity = true;
                    page->show();
                } break;
            }
        } else {
            auto scene = CCScene::create();
            if (targetScene == kSwitchToLast) {
                targetScene = g_eLastSwitch;
                g_eLastSwitch = g_eCurrentScene;
            } else if (targetScene != kSwitchToLastLevel) {
                g_eLastSwitch = g_eCurrentScene;
            }
            g_eCurrentScene = targetScene;
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
                    
                case kSwitchToSavedLevels:
                    scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(kGJSearchTypeSavedLevels)));
                    break;
                    
                case kSwitchToMapPacks:
                    scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(kGJSearchTypeMapPack)));
                    break;
                    
                case kSwitchToTheChallenge:
                    scene->release();
                    scene = PlayLayer::scene(GameLevelManager::sharedState()->getMainLevel(0xbb9, false));
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
                        switch (g_eLastLevelWasEdit) {
                            case kLastLevelTypeEditor:
                                scene->addChild(EditLevelLayer::create(g_pLastLevel));
                                break;
                            
                            case kLastLevelTypePlay:
                                scene->addChild(LevelInfoLayer::create(g_pLastLevel));
                                break;
                            
                            case kLastLevelTypeMain:
                                scene->addChild(LevelSelectLayer::create(g_pLastLevel->m_nLevelIndex));
                                break;
                        }
                    } else {
                        this->removeFromParent();
                        return;
                    }
                    break;
            }
            CCDirector::sharedDirector()->replaceScene(scene);
        }
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
