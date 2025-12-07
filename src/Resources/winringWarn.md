## WinRing0

PowerTuner service requires winring0 driver for some of its features,
like MSRs, fan control, ryzenadj, Intel MCHBAR.

This driver is now actively blocked by Windows and some modern anti-cheats because it uses vulnerable functions.

No valid or working alternative exists at the current time, but, as soon as a valid replacement
is available, PowerTuner will switch to it.

I'm actively looking for a replacement, or someone that can help make a new driver and has a way to sign it.

In the meantime, PowerTuner wont keep the driver loaded all the time, it loads/unloads it as needed.

**Internet connection** is required to download winring0 driver during install.

If no connection is available, you can download the driver files later:

[WinRing0x64.dll](https://github.com/PowerTuner/RyzenAdj/blob/main/win32/WinRing0x64.dll), [WinRing0x64.sys](https://github.com/PowerTuner/RyzenAdj/blob/main/win32/WinRing0x64.sys)

By checking "**I agree**", you allow PowerTuner installer to create a Windows defender exception for PowerTuner.
