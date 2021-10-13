#include <GDMake.h>
#include "Switcher.hpp"

GDMAKE_HOOK("libcocos2d.dll::?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z") GDMAKE_ATTR(NoLog)
void __fastcall CCKeyboardDispatcher_dispatchKeyboardMSG(
    CCKeyboardDispatcher* self,
    edx_t edx,
    enumKeyCodes key,
    bool down
) {
    if (key == KEY_Tab) {
        if (down) {
            Switcher::show();
        } else {
            Switcher::goTo();
        }
        return;
    }

    return GDMAKE_ORIG_V(self, edx, key, down);
}

GDMAKE_MAIN { return ""; }

GDMAKE_UNLOAD {}
