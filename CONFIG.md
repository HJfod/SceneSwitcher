# Configuring

You can configure SceneSwitcher by creating a file titled `SceneSwitcher.config` in your `Geometry Dash` directory (the one with the game executable).

The configuration file is written in plain text, and should be formatted as follows:

 * Any variables should be set on their own line using the pattern `x=v`, where `x` is the variable name and `v` is the value. You can set the value for multiple variables on the same line by separating them with a `,`, e.g. `x=v, x2=v2, x3=v3`. The format of value depends on the variable itself. Variable names are neither space-sensitive nor case-sensitive, so `Anim Speed`, `animspeed` and `anim speed` all refer to the same variable. Variables may also have alternative names.
 * The configuration of the actual menu buttons is done by writing each row on their own line, where items are separated by `|` and referred to by their name or an alternative. A row must have at least 1 item. Item names are neither space-sensitive nor case-sensitive, so `Main Menu`, `mainmenu` and `main men u` all refer to the same item.

The configuration file is loaded at startup and refreshed each time the menu is shown, unless the `No Refresh` variable is set to `1`.

### Example

```
key=Tab, animation=1, anim speed=10, trans=3, transition speed=.5, size=80:60, separation=90:70
Main Menu | Search
Quests | Profile | Settings | Weekly
```

### Variables

| Name(s)                         | Value                              | Format | Default Value | Description                                                      | Notes                                                                                                                                                                                                                                                                           |
|---------------------------------|------------------------------------|--------|---------------|------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Key                             | key code (int) / key name (string) | N / S  | Caps Lock     | The key that shows the menu.                                     | Only single keys allowed; no key combinations such as `Ctrl + Space` are possible. This option is only refreshed after the menu is shown, so after editing it, you will have to press the old key to show the menu and then the new one to close it (or just restart the game). |
| No Refresh, No Reload           | bool                               | B      | 0             | Disables reloading the config file every time the menu is shown. |                                                                                                                                                                                                                                                                                 |
| Size                            | Size                               | F:F    | 80.0:80.0     | Size of the items in the menu.                                   |                                                                                                                                                                                                                                                                                 |
| Separation, Sep, Spacing, Space | Size                               | F:F    | 90.0:90.0     | Spacing between items in the menu.                               | Relative to the item centres; does not take item size into account.                                                                                                                                                                                                             |
| Animation, Anim, Hover          | bool                               | B      | 0             | Enable / Disable item hover animation.                           |                                                                                                                                                                                                                                                                                 |
| Animation Speed, Anim Speed     | int                                | N      | 5             | Speed of the hover animation.                                    | In frames; not relative to framerate.                                                                                                                                                                                                                                           |
| Transition, Trans               | int                                | N      | 0             | Transition ID to use.                                            | See `Switcher.cpp::createTransitionFromArbitaryMagicNumber` for info on what numbers correspond to which transitions.                                                                                                                                                           |
| Transition Speed, Trans Speed   | float                              | F      | 0.5           | Speed of the transition.                                         |                                                                                                                                                                                                                                                                                 |

### Layers

| Name(s)                                                | Scene                                                                             |
|--------------------------------------------------------|-----------------------------------------------------------------------------------|
| Main Menu, MenuLayer, Menu                             | The main menu of GD                                                               |
| My Levels, Create, Local Levels                        | The user's created levels                                                         |
| Featured                                               | Featured levels                                                                   |
| Hall of Fame                                           | Hall of fame                                                                      |
| Gauntlets                                              | Gauntlets                                                                         |
| Leaderboards, Scores                                   | Top 100                                                                           |
| Official Levels, Main Levels, LevelSelectLayer         | Official levels                                                                   |
| Icon Kit, Garage Layer, GJGarageLayer, Garage          | Icon kit                                                                          |
| Online Levels, CreatorLayer, Online                    | Online levels (scene with the featured, create, quests etc. buttons)              |
| Saved Levels, Saved                                    | Saved levels                                                                      |
| Map Packs, Pain                                        | Map packs                                                                         |
| The Challenge                                          | The Challenge                                                                     |
| Search Levels, Search, Level Search, LevelSearchLayer  | Search levels                                                                     |
| Previous Scene, Last Scene, Prev Scene                 | Takes you to the last scene you were in before switching                          |
| Previous Level, Last Level, Prev Level                 | Takes you to the last level you viewed                                            |
| Settings, OptionsLayer                                 | Opens up GD settings (the popup with the options for account, options, help etc.) |
| Options, More Options, Options Popup, MoreOptionsLayer | Opens up GD settings (the popup with the actual options)                          |
| Profile, ProfilePage, User, My Account                 | Opens up your personal GD profile                                                 |
| Daily, Daily Level, DailyLevelPage                     | Opens up the daily level popup                                                    |
| Weekly, Weekly Level, Weekly Demon                     | Opens up the weekly demon popup                                                   |
| Quests, Challenges, ChallengesPage                     | Opens up quests                                                                   |

