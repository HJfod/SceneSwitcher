#pragma once

#include "utils.hpp"

enum SwitchTo {
    kSwitchToMainMenu,
    kSwitchToIconKit,
    kSwitchToOfficialLevels,
    kSwitchToMyLevels,
    kSwitchToOnlineLevels,
};

class Switch : public CCNode {
    protected:
        bool m_bHovered = false;
        SwitchTo m_eTo;

        bool init(const char* text, const char* spr, SwitchTo);

        void draw() override;
    
    public:
        static Switch* create(const char* text, const char* spr, SwitchTo, CCPoint const&, CCNode*);

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

    public:
        static Switcher* create();
        static void show();
        static void goTo();
};
