#include "Switcher.hpp"
#include "switchMacro.hpp"

// jaesus

SwitchTo g_eLastSwitch = kSwitchToMainMenu;
SwitchTo g_eCurrentScene = kSwitchToMainMenu;
GJGameLevel* g_pLastLevel = nullptr;
LastLevelType g_eLastLevelWasEdit = kLastLevelTypeMain;
enumKeyCodes g_eKey = KEY_CapsLock;
bool g_bAnimate = false;
int g_nAnimSpeed = 5;
bool g_bLoadOnlyAtStartup = false;
bool g_bHasBeenLoadedOnce = false;
int g_nArbitaryMagicTransitionNumber = 0;
float g_fTransitionSpeed = .5f;
std::vector<std::vector<SwitchTo>> g_vConfig;

CCSize Switcher::s_obItemSeparation = { 90.f, 90.f };
CCSize Switcher::s_obItemSize = { 80.f, 80.f };

static constexpr const char* g_sConfigFile = "SceneSwitcher.config";

constexpr const int SWITCHER_TAG = 0x517E5;

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
        case kSwitchToFeatured: return "Featured";
        case kSwitchToHallOfFame: return "Hall of Fame";
        case kSwitchToGauntlets: return "Gauntlets";
        case kSwitchToLeaderboards: return "Leaderboards";
        case kSwitchToDaily: return "Daily";
        case kSwitchToWeekly: return "Weekly";
        case kSwitchToLast: return "Last";
        case kSwitchToLastLevel: return "Last Level";
        case kSwitchToSettings: return "Settings";
        case kSwitchToMoreOptions: return "More Options";
        case kSwitchToProfile: return "Profile";
        case kSwitchToQuests: return "Quests";
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
        case kSwitchToMoreOptions: return "GJ_optionsBtn02_001.png";
        case kSwitchToFeatured: return "GJ_featuredBtn_001.png";
        case kSwitchToHallOfFame: return "GJ_fameBtn_001.png";
        case kSwitchToGauntlets: return "GJ_gauntletsBtn_001.png";
        case kSwitchToLeaderboards: return "GJ_highscoreBtn_001.png";
        case kSwitchToProfile: return "GJ_profileButton_001.png";
        case kSwitchToTheChallenge: return "difficulty_10_btn_001.png";
        case kSwitchToDaily: return "GJ_dailyBtn_001.png";
        case kSwitchToWeekly: return "GJ_weeklyBtn_001.png";
        case kSwitchToQuests: return "GJ_challengeBtn_001.png";
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

enumKeyCodes switchKey() {
    return g_eKey;
}

enumKeyCodes stringToKey(std::string const& s) {
    auto key = stringToLower(s);
    switch (hash(key.c_str())) {
        case hash("tab"):                           return KEY_Tab;
        case hash("caps"): case hash("capslock"):   return KEY_CapsLock;
        case hash("esc"): case hash("escape"):      return KEY_Escape;
        case hash("alt"):                           return KEY_Alt;
        case hash("shift"):                         return KEY_Shift;
        case hash("control"): case hash("ctrl"):    return KEY_Control;
        case hash("space"):                         return KEY_Space;
        case hash("enter"):                         return KEY_Enter;
    }
    try {
        auto k = std::stoi(key);
        return static_cast<enumKeyCodes>(k);
    } catch (...) {}
    return KEY_CapsLock;
}

CCScene* createTransitionFromArbitaryMagicNumber(CCScene* scene, int num) {
    #define ARBITARY_SCENE(trans) case __LINE__ - 121: return trans::create(g_fTransitionSpeed, scene)
    #define ARBITARY_SCEN3(trans, b) case __LINE__ - 121: return trans::create(g_fTransitionSpeed, scene, b)

    switch (num) {
        case 0: return scene;
        ARBITARY_SCENE(CCTransitionFade);           // 1
        ARBITARY_SCENE(CCTransitionCrossFade);
        ARBITARY_SCENE(CCTransitionFadeBL);
        ARBITARY_SCENE(CCTransitionFadeTR);
        ARBITARY_SCEN3(CCTransitionPageTurn, true); // 5
        ARBITARY_SCEN3(CCTransitionPageTurn, false);
        ARBITARY_SCENE(CCTransitionRotoZoom);       
        ARBITARY_SCENE(CCTransitionJumpZoom);
        ARBITARY_SCENE(CCTransitionMoveInL);        // 10
        ARBITARY_SCENE(CCTransitionMoveInR);        
        ARBITARY_SCENE(CCTransitionMoveInT);        
        ARBITARY_SCENE(CCTransitionMoveInB);
        ARBITARY_SCENE(CCTransitionSlideInL);       // 15
        ARBITARY_SCENE(CCTransitionSlideInR);       
        ARBITARY_SCENE(CCTransitionSlideInT);       
        ARBITARY_SCENE(CCTransitionSlideInB);
        ARBITARY_SCENE(CCTransitionShrinkGrow);     // 20
        ARBITARY_SCENE(CCTransitionFlipX);          
        ARBITARY_SCENE(CCTransitionFlipY);
        ARBITARY_SCENE(CCTransitionSplitCols);
        ARBITARY_SCENE(CCTransitionSplitRows);      // 25
        ARBITARY_SCENE(CCTransitionFadeUp);         
        ARBITARY_SCENE(CCTransitionFadeDown);
    }

    return scene;
}


bool Switch::init(const char* text, const char* sprName, SwitchTo to, float scale) {
    if (!CCNode::init())
        return false;
    
    this->m_eTo = to;

    this->setContentSize(Switcher::s_obItemSize);

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
        if (g_bAnimate) {
            if (m_nAnim + g_nAnimSpeed < m_nAnimTarget) {
                m_nAnim += g_nAnimSpeed;
            } else {
                m_nAnim = m_nAnimTarget;
            }
        } else {
            m_nAnim = m_nAnimTarget;
        }
    } else {
        if (g_bAnimate) {
            if (m_nAnim - g_nAnimSpeed > 0) {
                m_nAnim -= g_nAnimSpeed;
            } else {
                m_nAnim = 0;
            }
        } else {
            m_nAnim = 0;
        }
    }
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1, 1, 1, m_nAnim / 255.f });

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
        auto p = pos + CCDirector::sharedDirector()->getWinSize() / 2;
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
        case kSwitchToQuests:
        case kSwitchToMoreOptions:
            return true;
    }
    return false;
}

bool Switcher::loadConfigVars(std::string const& line) {
    std::vector<std::string> data;
    if (!line.size()) {
        if (std::filesystem::exists(g_sConfigFile)) {
            auto fdata = readFileString(g_sConfigFile);
            while (stringContains(fdata, " "))
                fdata = stringReplace(fdata, " ", "");
            while (stringContains(fdata, "\r"))
                fdata = stringReplace(fdata, "\r", "");
            fdata = stringToLower(fdata);
            
            data = stringSplit(fdata, "\n");
        }
    } else {
        data = { line };
    }
    bool ret = false;
    for (auto const& line : data) {
        if (stringContains(line, "=")) {
            for (auto const& item_r : stringSplit(line, ",")) {
                auto var = item_r.substr(0, item_r.find_first_of("="));
                auto val = item_r.substr(item_r.find_first_of("=") + 1);
                switch (hash(var.c_str())) {
                    case hash("key"): {
                        g_eKey = stringToKey(val);
                        ret = true;
                    } break;

                    case hash("norefresh"): case hash("noreload"): {
                        g_bLoadOnlyAtStartup = std::stoi(val);
                        ret = true;
                    } break;

                    case hash("animspeed"): case hash("animationspeed"): {
                        g_nAnimSpeed = std::stoi(val);
                        ret = true;
                    } break;

                    case hash("anim"): case hash("animation"): {
                        g_bAnimate = std::stoi(val);
                        ret = true;
                    } break;

                    case hash("transitionspeed"): case hash("transspeed"):
                    case hash("nottheuk"): {
                        g_fTransitionSpeed = std::stof(val);
                        ret = true;
                    } break;

                    case hash("transition"): case hash("trans"): case hash("nb"): {
                        g_nArbitaryMagicTransitionNumber = std::stoi(val);
                        ret = true;
                    } break;

                    case hash("size"): {
                        if (stringContains(item_r, ":")) {
                            try {
                                auto x = std::stof(val.substr(0, val.find_first_of(":")));
                                auto y = std::stof(val.substr(val.find_first_of(":") + 1));
                                Switcher::s_obItemSize = CCSize { x, y };
                                ret = true;
                            } catch (...) {}
                        }
                    } break;

                    case hash("sep"): case hash("separation"):
                    case hash("space"): case hash("spacing"): {
                        if (stringContains(item_r, ":")) {
                            try {
                                auto x = std::stof(val.substr(0, val.find_first_of(":")));
                                auto y = std::stof(val.substr(val.find_first_of(":") + 1));
                                Switcher::s_obItemSize = CCSize { x, y };
                                ret = true;
                            } catch (...) {}
                        }
                    } break;
                }
            }
        }
    }
    return ret;
}

decltype(Switcher::m_vConfig) Switcher::loadConfigFile() {
    if (g_bLoadOnlyAtStartup && g_bHasBeenLoadedOnce)
        return this->m_vConfig;
    decltype(Switcher::m_vConfig) cfg;
    if (std::filesystem::exists(g_sConfigFile)) {
        g_bHasBeenLoadedOnce = true;
        auto data = readFileString(g_sConfigFile);
        if (this->m_sRawConfigFileData == data)
            return this->m_vConfig;
        this->m_sRawConfigFileData = data;
        for (auto const& line_r : stringSplit(data, "\n")) {
            auto line = line_r;
            while (stringContains(line, " "))
                line = stringReplace(line, " ", "");
            while (stringContains(line, "\r"))
                line = stringReplace(line, "\r", "");
            line = stringToLower(line);

            if (this->loadConfigVars(line))
                continue;

            cfg.push_back(std::vector<SwitchTo>());

            for (auto const& item_r : stringSplit(line, "|")) {
                switch (hash(item_r.c_str())) {
                    RES(
                        kSwitchToMainMenu,
                        "mainmenu", "menulayer", "menu"
                    );
                    RES(
                        kSwitchToMyLevels,
                        "mylevels"
                    );
                    RES(
                        kSwitchToFeatured,
                        "featured"
                    );
                    RES(
                        kSwitchToHallOfFame,
                        "halloffame"
                    );
                    RES(
                        kSwitchToGauntlets,
                        "gauntlets"
                    );
                    RES(
                        kSwitchToLeaderboards,
                        "leaderboards", "scores"
                    );
                    RES(
                        kSwitchToOfficialLevels,
                        "officiallevels", "mainlevels", "levelselectlayer"
                    );
                    RES(
                        kSwitchToIconKit,
                        "iconkit", "garagelayer", "gjgaragelayer", "garage"
                    );
                    RES(
                        kSwitchToOnlineLevels,
                        "onlinelevels", "creatorlayer", "online"
                    );
                    RES(
                        kSwitchToSavedLevels,
                        "savedlevels", "saved"
                    );
                    RES(
                        kSwitchToMapPacks,
                        "mappacks", "pain"
                    );
                    RES(
                        kSwitchToTheChallenge,
                        "thechallenge", "challenge", "epic"
                    );
                    RES(
                        kSwitchToSearch,
                        "searchlevels", "search", "levelsearch", "levelsearchlayer"
                    );
                    RES(
                        kSwitchToLast,
                        "previousscene", "lastscene", "prevscene"
                    );
                    RES(
                        kSwitchToLastLevel,
                        "prevlevel", "lastlevel", "previouslevel"
                    );
                    RES(
                        kSwitchToSettings,
                        "optionslayer", "settings"
                    );
                    RES(
                        kSwitchToMoreOptions,
                        "options", "moreoptions", "optionspopup"
                    );
                    RES(
                        kSwitchToProfile,
                        "profile", "profilepage", "user", "myaccount"
                    );
                    RES(
                        kSwitchToDaily,
                        "daily", "dailylevel", "dailylevelpage"
                    );
                    RES(
                        kSwitchToWeekly,
                        "weekly", "weeklylevel", "weeklylevelpage"
                    );
                    RES(
                        kSwitchToQuests,
                        "quests", "challenges"
                    );
                }
            }
        }
    }
    return cfg;
}

void Switcher::loadConfig(decltype(Switcher::m_vConfig) cfg) {
    for (auto const& s : this->m_vSwitches) {
        s->removeFromParent();
    }
    this->m_vSwitches.clear();

    this->m_vConfig = cfg;

    auto c_ix = 0;
    for (auto const& c : cfg) {
        auto h_ix = 0;
        for (auto const& h : c) {
            auto posx = s_obItemSeparation.width *
                (static_cast<float>(h_ix % c.size()) - c.size() / 2.f);
            auto posy = s_obItemSeparation.height *
                ((cfg.size()) / 2.f - static_cast<float>(c_ix % cfg.size()) - 1);
            switch (h) {
                case kSwitchToLastLevel: {
                    auto lastLevel = g_pLastLevel ? 
                        CCString::createWithFormat("Last Level (%s)", g_pLastLevel->m_sLevelName.c_str())->getCString() :
                        "Last Level (None)";
                    auto diffSpr = g_pLastLevel ?
                        diffToSprName(g_pLastLevel) :
                        "difficulty_00_btn_001.png";

                    this->m_vSwitches.push_back(
                        Switch::create(lastLevel, diffSpr, h, { posx, posy }, this)
                    );
                } break;

                case kSwitchToLast: {
                    auto str = CCString::createWithFormat(
                        "Last Scene (%s)",
                        SwitchToToString(g_eLastSwitch)
                    );
                    this->m_vSwitches.push_back(
                        Switch::create(
                            str->getCString(),
                            SwitchToToSprName(g_eLastSwitch),
                            h, { posx, posy }, this
                        )
                    );
                } break;

                case kSwitchToSettings:
                case kSwitchToProfile: {
                    this->m_vSwitches.push_back(
                        Switch::create(
                            nullptr, nullptr,
                            h,
                            { posx, posy },
                            this, .8f
                        )
                    );
                } break;

                default: {
                    this->m_vSwitches.push_back(
                        Switch::create(
                            nullptr, nullptr,
                            h,
                            { posx, posy },
                            this
                        )
                    );
                }
            }
            h_ix++;
        }
        c_ix++;
    }
}

bool Switcher::init() {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 230 }))
        return false;

    this->setTag(SWITCHER_TAG);

    m_vConfig = g_vConfig;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto titleLabel = CCLabelBMFont::create("Go To Layer", "bigFont.fnt");
    titleLabel->setPosition(winSize.width / 2, winSize.height - 15.f);
    titleLabel->setScale(.4f);
    this->addChild(titleLabel);

    auto fileCfg = this->loadConfigFile();

    if (fileCfg.size()) {
        this->loadConfig(fileCfg);
    } else {
        this->loadConfig({
            { kSwitchToMainMenu, kSwitchToMyLevels, kSwitchToOnlineLevels, kSwitchToIconKit, },
            { kSwitchToSearch, kSwitchToSavedLevels, kSwitchToDaily, kSwitchToWeekly, },
            { kSwitchToSettings, kSwitchToProfile, kSwitchToLastLevel, kSwitchToLast, },
        });
    }

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

                case kSwitchToMoreOptions:
                    MoreOptionsLayer::create()->show();
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

                case kSwitchToQuests: {
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
            CCDirector::sharedDirector()->replaceScene(
                createTransitionFromArbitaryMagicNumber(
                    scene, g_nArbitaryMagicTransitionNumber
                )
            );
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

Switcher::~Switcher() {
    g_vConfig = m_vConfig;
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
