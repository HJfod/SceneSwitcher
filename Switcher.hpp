#pragma once

#include "utils.hpp"

enum SwitchTo {
    kSwitchToMainMenu,
    kSwitchToIconKit,
    kSwitchToOfficialLevels,
    kSwitchToMyLevels,
    kSwitchToOnlineLevels,
    kSwitchToSavedLevels,
    kSwitchToMapPacks,
    kSwitchToTheChallenge,
    kSwitchToFeatured,
    kSwitchToHallOfFame,
    kSwitchToGauntlets,
    kSwitchToLeaderboards,
    kSwitchToSearch,
    kSwitchToDaily,
    kSwitchToWeekly,
    kSwitchToLast,
    kSwitchToLastLevel,
    kSwitchToSettings,
    kSwitchToMoreOptions,
    kSwitchToProfile,
    kSwitchToQuests,
};
constexpr const char* SwitchToToString(SwitchTo to);

enum LastLevelType {
    kLastLevelTypeMain,
    kLastLevelTypeEditor,
    kLastLevelTypePlay,
};

void setLastViewedLevel(GJGameLevel*, LastLevelType);
enumKeyCodes switchKey();

class Switch : public CCNode {
    protected:
        bool m_bHovered = false;
        SwitchTo m_eTo;
        GLubyte m_nAnimTarget = 51;
        GLubyte m_nAnim = 0;

        bool init(const char* text, const char* spr, SwitchTo, float size);

        void draw() override;
    
    public:
        static Switch* create(const char* text, const char* spr, SwitchTo, CCPoint const&, CCNode*, float size = 1.f);

        void updateHover(CCPoint const&);
        bool isHovered() const;
        SwitchTo getWhere() const;
};

class Switcher : public CCLayerColor {
    public:
        static CCSize s_obItemSeparation;
        static CCSize s_obItemSize;

    protected:
        std::vector<Switch*> m_vSwitches;
        std::vector<std::vector<SwitchTo>> m_vConfig;
        std::string m_sRawConfigFileData = "";

        void loadConfig(decltype(m_vConfig));
        bool init();
        void visit() override;
        void go();
        static constexpr bool isPopupSwitchType(SwitchTo);

        bool handle(enumKeyCodes);

        virtual ~Switcher();

    public:
        decltype(m_vConfig) loadConfigFile();
        static bool handleKey(enumKeyCodes);

        static bool loadConfigVars(std::string const& = "");
        static Switcher* create();
        static void show();
        static void goTo();
};
