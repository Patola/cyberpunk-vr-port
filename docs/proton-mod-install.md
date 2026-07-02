# Proton mod install notes

These notes record the currently tested Linux/Proton setup path for the
Cyberpunk 2077 mod dependencies used by this fork. Keep this file practical:
prefer launch options and visible files over hidden prefix changes whenever
possible.

## Baseline

Tested locally on Steam/Proton with Cyberpunk 2077 installed at:

```text
/nvme/SteamLibrary/steamapps/common/Cyberpunk 2077/
```

The game was first launched once without mods to create the Proton prefix:

```text
/nvme/SteamLibrary/steamapps/compatdata/1091500/pfx
```

Recommended early test graphics state:

- borderless window mode;
- Steam overlay disabled;
- upscaling disabled;
- ray tracing/path tracing disabled;
- frame generation disabled;
- film grain disabled.

## Steam launch options

For CET and RED4ext together:

```sh
WINEDLLOVERRIDES="version,winmm=n,b" PROTON_LOG=1 %command% --launcher-skip
```

Known DLL override users:

- `version=n,b`: Cyber Engine Tweaks loader.
- `winmm=n,b`: RED4ext loader.

When this fork's `dxgi.dll` is added later, the launch option is expected to
become:

```sh
WINEDLLOVERRIDES="version,winmm,dxgi=n,b" PROTON_LOG=1 %command% --launcher-skip
```

Prefer this environment-variable form over editing Wine registry DLL overrides.
It is visible in Steam launch options, easy to copy into public instructions,
and easy to remove.

## Proton prefix components

Cyber Engine Tweaks requires a recent Visual C++ 2015-2022 runtime. The tested
prefix had an older 14.28 runtime and CET failed while loading
`cyber_engine_tweaks.asi` with Error 998 / an access violation inside
`MSVCP140.dll`.

The working fix was:

```sh
protontricks 1091500 -q vcrun2022
```

After installation, the prefix reported Visual C++ 2015-2022 runtime
`14.44.35211`, satisfying CET's `>= 14.40.33810.0` requirement.

`d3dcompiler_47.dll` was already present in this prefix. If CET fails later with
a missing `D3DCOMPILER_47.dll` import, install it with protontricks/winetricks
for the same app id.

## 1. Cyber Engine Tweaks

Source:

```text
https://wiki.redmodding.org/cyber-engine-tweaks/getting-started/installing
```

Install method:

- download the release zip, not source code;
- extract the archive into the Cyberpunk 2077 game directory.

Expected files:

```text
Cyberpunk 2077/
  bin/
    x64/
      global.ini
      version.dll
      plugins/
        cyber_engine_tweaks.asi
        cyber_engine_tweaks/
```

Required launch option:

```sh
WINEDLLOVERRIDES="version=n,b" %command% --launcher-skip
```

Verification:

```text
bin/x64/plugins/cyber_engine_tweaks/cyber_engine_tweaks.log
bin/x64/plugins/cyber_engine_tweaks/scripting.log
```

Observed working versions:

- CET `v1.37.1 [HEAD]`;
- Cyberpunk 2077 game version `3.0.80.51928`;
- first launch prompted for the CET console keybind;
- assigned key: backtick.

## 2. RED4ext

Source:

```text
https://github.com/WopsS/RED4ext
```

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Required launch option:

```sh
WINEDLLOVERRIDES="winmm=n,b" %command% --launcher-skip
```

Use it together with CET as:

```sh
WINEDLLOVERRIDES="version,winmm=n,b" %command% --launcher-skip
```

Verification:

```text
red4ext/logs/red4ext-*.log
```

Observed working versions:

- RED4ext `v1.30.0`;
- Cyberpunk 2077 product version `2.31`;
- Cyberpunk 2077 file version `3.0.80.51928`;
- log reported: `RED4ext has been successfully initialized`.

## Next

Continue installing and verifying the dependency chain in order:

1. ArchiveXL
2. TweakXL
3. redscript
4. Codeware
5. Visual Holsters (Automatic Clothes Swap)
6. Visible Bullets (Projectile Restoration)
7. Equipment-EX
8. This fork's `dxgi.dll` and `CyberpunkVR_Hands.dll`
