# Third-party components

## Original Road Rash

Original game program, artwork, sound and icon assets remain the property of their original rights holders. They are not covered by this project's MIT license. This is an unofficial compatibility/modification project; no original-game redistribution grant is supplied by this repository.

## cnc-ddraw 7.1.0.0

Upstream: https://github.com/FunkyFr3sh/cnc-ddraw/tree/v7.1.0.0

MIT license: `licenses/cnc-ddraw-MIT.txt`. Complete modified source is in `src/cnc-ddraw/` and in the playable packages. Included Detours, LodePNG and graphics headers retain their embedded notices and licenses.

## Wine 11.0 / macOS build 11.0_1

Wine is dynamically loaded from the replaceable runtime directory inside the Mac app; the project does not modify Wine code.

- Upstream Wine source: https://github.com/wine-mirror/wine/tree/wine-11.0
- Source archive: https://github.com/wine-mirror/wine/archive/refs/tags/wine-11.0.tar.gz
- Runtime distributor: https://github.com/Gcenx/macOS_Wine_builds/releases/tag/11.0_1
- Wine license: `licenses/Wine-LGPL-2.1.txt`.

The macOS runtime additionally includes third-party shared libraries supplied by that distributor, including SDL2, MoltenVK, MacPorts legacy support, compression libraries, font/image libraries, ICU, libffi, GnuTLS and its dependencies. They retain their respective upstream licenses and are not relicensed under this project's MIT license. Runtime build/source provenance is available from the linked distributor and corresponding upstream projects. This project does not change those library binaries.

## Build tools and research reference

- llvm-mingw compiler used for the current DLL: https://github.com/mstorsjo/llvm-mingw/releases/tag/20261006
- Road Rash structure research reference: https://github.com/trapexit/3do-decomp-road-rash

PC-specific instruction signatures, hook locations and added behavior were checked against this project's executable. The project does not claim ownership of the original game.
