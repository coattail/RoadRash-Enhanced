#!/bin/bash
set -eu
ROOT="$(cd "$(dirname "$0")" && pwd)"
SUPPORT="$HOME/Library/Application Support/RoadRashMac"
export WINEPREFIX="$SUPPORT/Prefix"
export WINEDEBUG="${ROADRASH_DEBUG:--all,err+all}"
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
export PATH="$ROOT/wine/bin:/usr/bin:/bin:/usr/sbin:/sbin"
export WINESERVER="$ROOT/wine/bin/wineserver"
export WINEDLLPATH="$ROOT/wine/lib/wine"
export WINELOADER="$ROOT/GameRuntime.app/Contents/MacOS/wine"
export MVK_CONFIG_LOG_LEVEL=0
export WINEDLLOVERRIDES='ddraw=n,b;dsound=b;mscoree,mshtml=;winemenubuilder.exe='
WINE="$ROOT/wine/bin/wine"
SERVER="$ROOT/wine/bin/wineserver"
mkdir -p "$SUPPORT" "$HOME/Library/Logs/RoadRashMac"
exec >> "$HOME/Library/Logs/RoadRashMac/game.log" 2>&1
printf '\n--- %s ---\n' "$(date)"
printf 'preparing\n' > "$SUPPORT/run.state"
if ! /usr/bin/arch -x86_64 /usr/bin/true; then exit 10; fi
trap '"$SERVER" -k >/dev/null 2>&1 || true; exit 0' TERM INT HUP
if [ ! -f "$SUPPORT/.ready-v1" ]; then
  "$WINE" wineboot -u
  "$SERVER" -w
  if [ ! -d "$WINEPREFIX/drive_c/RoadRash" ]; then
    /usr/bin/ditto "$ROOT/Game" "$WINEPREFIX/drive_c/RoadRash"
  fi
  "$WINE" regedit /S "$ROOT/roadrash.reg"
  "$SERVER" -w
  touch "$SUPPORT/.ready-v1"
fi
if [ ! -f "$SUPPORT/.input-fix-v2" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  /bin/cp "$ROOT/Game/ddraw.ini" "$WINEPREFIX/drive_c/RoadRash/ddraw.ini"
  "$WINE" regedit /S "$ROOT/input-fix.reg"
  "$SERVER" -w
  touch "$SUPPORT/.input-fix-v2"
fi
if [ ! -f "$SUPPORT/.mouse-free-v3" ]; then
  /bin/cp "$ROOT/Game/ddraw.ini" "$WINEPREFIX/drive_c/RoadRash/ddraw.ini"
  touch "$SUPPORT/.mouse-free-v3"
fi
if [ ! -f "$SUPPORT/.resize-v4" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  /bin/cp "$ROOT/Game/ddraw.ini" "$WINEPREFIX/drive_c/RoadRash/ddraw.ini"
  touch "$SUPPORT/.resize-v4"
fi
if [ ! -f "$SUPPORT/.aspect-v5" ]; then
  /bin/cp "$ROOT/Game/ddraw.ini" "$WINEPREFIX/drive_c/RoadRash/ddraw.ini"
  touch "$SUPPORT/.aspect-v5"
fi
if [ ! -f "$SUPPORT/.mouse-hitboxes-v6" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.mouse-hitboxes-v6"
fi
if [ ! -f "$SUPPORT/.aggressive-riders-v7" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.aggressive-riders-v7"
fi
if [ ! -f "$SUPPORT/.aggressive-pursuit-v8" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.aggressive-pursuit-v8"
fi
if [ ! -f "$SUPPORT/.aggressive-pursuit-v9" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.aggressive-pursuit-v9"
fi
if [ ! -f "$SUPPORT/.combat-spacing-v10" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.combat-spacing-v10"
fi
if [ ! -f "$SUPPORT/.counterattack-v11" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.counterattack-v11"
fi
if [ ! -f "$SUPPORT/.player-celebration-v12" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.player-celebration-v12"
fi
if [ ! -f "$SUPPORT/.mod-mapping-v13" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  touch "$SUPPORT/.mod-mapping-v13"
fi
if [ ! -f "$SUPPORT/.gameplay-module-v14" ]; then
  /bin/cp "$ROOT/Game/Roadrash.exe" "$WINEPREFIX/drive_c/RoadRash/Roadrash.exe"
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.gameplay-module-v14"
fi
if [ ! -f "$SUPPORT/.pause-audio-v15" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.pause-audio-v15"
fi
if [ ! -f "$SUPPORT/.fall-animation-v16" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.fall-animation-v16"
fi
if [ ! -f "$SUPPORT/.dashboard-v17" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.dashboard-v17"
fi
if [ ! -f "$SUPPORT/.counterhit-v18" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.counterhit-v18"
fi
if [ ! -f "$SUPPORT/.counter-priority-v19" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.counter-priority-v19"
fi
if [ ! -f "$SUPPORT/.defense-v20" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.defense-v20"
fi
if [ ! -f "$SUPPORT/.defense-v21" ]; then
  /bin/cp "$ROOT/Game/ddraw.dll" "$WINEPREFIX/drive_c/RoadRash/ddraw.dll"
  touch "$SUPPORT/.defense-v21"
fi
cd "$WINEPREFIX/drive_c/RoadRash"
printf 'ready\n' > "$SUPPORT/run.state"
WINELOADERNOEXEC=1 "$ROOT/GameRuntime.app/Contents/MacOS/wine" 'C:\RoadRash\Roadrash.exe' &
GAME_PID=$!
set +e
wait "$GAME_PID"
GAME_STATUS=$?
printf "Game exit status: %s\n" "$GAME_STATUS"
set -e
"$SERVER" -k >/dev/null 2>&1 || true
exit "$GAME_STATUS"
