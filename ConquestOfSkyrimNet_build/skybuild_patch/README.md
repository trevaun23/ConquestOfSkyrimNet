# SkyBuild navmesh patch

SkyBuild can rebuild the navmesh only from its own hammer wheel. This patch adds Papyrus functions to
`SkyBuildNative` (RequestNavmesh, NavmeshStage, NavmeshBusy, NavmeshProgress, NavmeshMessage, NavmeshApiVersion)
so the Settlements tab's **Update navmesh** button can start it and show progress.

1. Install Git, Visual Studio 2022 with "Desktop development with C++", CMake and vcpkg (set `VCPKG_ROOT`).
2. Run `build_skybuild_patched.bat`. It clones SkyBuild at the commit in `BASE_COMMIT.txt`, applies
   `skybuild-navmesh-papyrus.patch` and builds Release.
3. Copy the new `SkyBuild.dll` over `Data\SKSE\Plugins\SkyBuild.dll` (back up the original).

If the preset names differ in a newer SkyBuild, open `CMakePresets.json` and use the release preset listed there.
SkyBuild is GPL-3.0; the patch is under the same licence.
