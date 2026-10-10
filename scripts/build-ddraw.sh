#!/bin/bash
set -euo pipefail
RR_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RR_CC="${RR_TOOLCHAIN_BIN:+$RR_TOOLCHAIN_BIN/}i686-w64-mingw32-gcc"
RR_WINDRES="${RR_TOOLCHAIN_BIN:+$RR_TOOLCHAIN_BIN/}i686-w64-mingw32-windres"
command -v "$RR_CC" >/dev/null
command -v "$RR_WINDRES" >/dev/null
mkdir -p "$RR_ROOT/build"
make -C "$RR_ROOT/src/cnc-ddraw" CC="$RR_CC" WINDRES="$RR_WINDRES"   LDFLAGS="-Wl,--enable-stdcall-fixup,-Map,$RR_ROOT/build/roadrash-ddraw.map -s -static -shared"
