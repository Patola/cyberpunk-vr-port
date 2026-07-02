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

For the current full dependency stack plus this fork's `dxgi.dll`:

```sh
WINEDLLOVERRIDES="version,winmm,dxgi=n,b" PROTON_LOG=1 %command% --launcher-skip
```

Known DLL override users:

- `version=n,b`: Cyber Engine Tweaks loader.
- `winmm=n,b`: RED4ext loader.
- `dxgi=n,b`: this fork's OpenXR/DXGI proxy.

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

## 3. ArchiveXL

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- RED4ext plugin;
- no additional `WINEDLLOVERRIDES` entry was needed beyond RED4ext's
  `winmm=n,b`.

Verification:

```text
red4ext/plugins/ArchiveXL/ArchiveXL-*.log
```

Observed working version:

- ArchiveXL `1.26.8`;
- log reported: `All archive extensions loaded.`;
- log showed resource patch processing during startup.

## 4. TweakXL

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- RED4ext plugin;
- no additional `WINEDLLOVERRIDES` entry was needed beyond RED4ext's
  `winmm=n,b`.

Verification:

```text
red4ext/plugins/TweakXL/TweakXL-*.log
```

Observed working version:

- TweakXL `1.11.3`;
- log reported inheritance metadata and extra flats metadata loading;
- log reported tweak scanning completed.

## 5. redscript

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- game script compiler integration;
- no additional `WINEDLLOVERRIDES` entry was needed.

Verification:

```text
r6/logs/redscript_rCURRENT.log
r6/cache/final.redscripts.modded
```

Observed working result:

- log reported compilation of ArchiveXL and TweakXL `.reds` scripts;
- log reported: `Compilation complete`;
- log reported output saved to `r6/cache/final.redscripts.modded`.

## 6. Codeware

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- RED4ext plugin;
- no additional `WINEDLLOVERRIDES` entry was needed beyond RED4ext's
  `winmm=n,b`.

Verification:

```text
red4ext/plugins/Codeware/Codeware-*.log
```

Observed working version:

- Codeware `1.20.3`;
- log reported: `Codeware is initialized.`;
- log reported `127623` predefined resource path hashes loaded.

## 7. Visual Holsters (Automatic Clothes Swap)

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- CET mod;
- no additional `WINEDLLOVERRIDES` entry was needed.

Verification:

```text
bin/x64/plugins/cyber_engine_tweaks/mods/VisualHolster/VisualHolster.log
```

Observed working version:

- Visual Holster `1.2 REL`;
- before Equipment-EX was installed, the log warned that Equipment-EX was
  required;
- after Equipment-EX was installed, the log reported initialization with
  `Slots: 31`, `Tags: 91`, `Custom Items: 0`, `EquipmentEx Version: 1.2.9`.

## 8. Visible Bullets (Projectile Restoration)

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- TweakXL tweaks;
- no additional `WINEDLLOVERRIDES` entry was needed.

Observed install footprint:

```text
r6/tweaks/BulletProjectileOverhaul/
```

## 9. Equipment-EX

Install method:

- download the release zip;
- extract the archive into the Cyberpunk 2077 game directory.

Loader:

- ArchiveXL archive;
- redscript scripts;
- no additional `WINEDLLOVERRIDES` entry was needed.

Observed install footprint:

```text
archive/pc/mod/EquipmentEx.archive
archive/pc/mod/EquipmentEx.archive.xl
r6/config/redsUserHints/EquipmentEx.toml
r6/scripts/EquipmentEx/
```

Observed working version:

- Equipment-EX `1.2.9`, as reported by Visual Holster after initialization.

## Optional holster content packs

The local test also installed extra holster content used by Visual Holster:

```text
archive/pc/mod/s10_eqkatana.archive
archive/pc/mod/s10_katanaeqh.archive.xl
archive/pc/mod/scorpion_military_dual_pistol_holsters.archive
archive/pc/mod/scorpion_military_dual_pistol_holsters.xl
```

These were extracted into the Cyberpunk 2077 game directory and did not require
additional `WINEDLLOVERRIDES` entries.

## 10. Cyberpunk VR Port fork

Build both DLLs with:

```sh
./scripts/build-msvc-clang.sh
```

Install files:

```text
build-msvc-clang-dxgi/bin/dxgi.dll
  -> Cyberpunk 2077/bin/x64/dxgi.dll

build-msvc-clang-hands/CyberpunkVR_Hands.dll
  -> Cyberpunk 2077/red4ext/plugins/CyberpunkVR_Hands/CyberpunkVR_Hands.dll

mods/cet/CyberpunkVRPort_*
  -> Cyberpunk 2077/bin/x64/plugins/cyber_engine_tweaks/mods/

mods/redscript/CyberpunkVRPort_*
  -> Cyberpunk 2077/r6/scripts/
```

Required launch option with CET and RED4ext:

```sh
WINEDLLOVERRIDES="version,winmm,dxgi=n,b" PROTON_LOG=1 %command% --launcher-skip
```

Expected first-run logs:

```text
bin/x64/cyberpunkvrport.log
red4ext/plugins/CyberpunkVR_Hands/
```

## Next

Continue installing and verifying the dependency chain in order:

1. Launch with `WINEDLLOVERRIDES="version,winmm,dxgi=n,b"`.
2. Check `bin/x64/cyberpunkvrport.log`.
3. Check RED4ext logs for `CyberpunkVR_Hands.dll`.
4. Start in conservative graphics settings before enabling optional VR features.
