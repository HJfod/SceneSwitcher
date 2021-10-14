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
    kSwitchToSearch,
    kSwitchToDaily,
    kSwitchToWeekly,
    kSwitchToLast,
    kSwitchToLastLevel,
    kSwitchToSettings,
    kSwitchToProfile,
};
constexpr const char* SwitchToToString(SwitchTo to);

enum LastLevelType {
    kLastLevelTypeMain,
    kLastLevelTypeEditor,
    kLastLevelTypePlay,
};

void setLastViewedLevel(GJGameLevel*, LastLevelType);

class Switch : public CCNode {
    protected:
        bool m_bHovered = false;
        SwitchTo m_eTo;

        bool init(const char* text, const char* spr, SwitchTo, float size);

        void draw() override;
    
    public:
        static Switch* create(const char* text, const char* spr, SwitchTo, CCPoint const&, CCNode*, float size = 1.f);

        void updateHover(CCPoint const&);
        bool isHovered() const;
        SwitchTo getWhere() const;
};

class Switcher : public CCLayerColor {
    protected:
        std::vector<Switch*> m_vSwitches;

        bool init();
        void visit() override;
        void go();
        static constexpr bool isPopupSwitchType(SwitchTo);

        bool handle(enumKeyCodes);

    public:
        static bool handleKey(enumKeyCodes);

        static Switcher* create();
        static void show();
        static void goTo();
};
