#!/usr/bin/env bash
# ビルド → アセット再コンパイル(pdc) → シミュレータ再起動 を一発で。
#
# 注意: `cmake --build` は C のコードが変わった時しか pdc を再実行しないため、
# 画像・音などアセットだけ変えても pdx に反映されないことがある。
# このスクリプトは毎回 pdc を明示実行して確実に最新アセットを取り込み、
# シミュレータも一度終了してから起動し直す（古い表示のキャッシュ回避）。
#
#   ./run.sh
set -e
cd "$(dirname "$0")"
export PLAYDATE_SDK_PATH="${PLAYDATE_SDK_PATH:-$HOME/Developer/PlaydateSDK}"

cmake -B build -S . >/dev/null
cmake --build build
"$PLAYDATE_SDK_PATH/bin/pdc" -sdkpath "$PLAYDATE_SDK_PATH" Source myfirstgame.pdx

killall "Playdate Simulator" 2>/dev/null || true
sleep 1
open -a "$PLAYDATE_SDK_PATH/bin/Playdate Simulator.app" myfirstgame.pdx
echo "launched myfirstgame.pdx"
