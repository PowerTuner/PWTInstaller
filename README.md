# PowerTuner installer

![](src/Resources/icon.ico)

PowerTuner installer for Windows.

## Winring0

PowerTuner requires _winring0_ driver on Windows for some of its features:

- MSRs
- MCHBAR
- RyzenAdj
- Fan control

The driver is now actively blocked by Windows and recent anti-cheats,
hence why the decision to **NOT** bundle _winring0_ in the installer itself.

_winring0_ will be downloaded during install instead.

This makes the installer a clean application that wont be blocked by _Windows defender_ or _Smart screen_.

Unfortunately, no valid alternative exists right now that can replace that driver,
help is appreciated to find or create and sign a new one, to replace _winring0_, that is secure and anti-cheat friendly.
