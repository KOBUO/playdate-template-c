#!/usr/bin/env python3
"""
ランチャー用アセットを生成して Source/SystemAssets/ に置く。
  - launchImage.png : 400x240（起動時に表示）
  - card.png        : 350x155（ランチャーのカード）
タイトル画像(Source/images/title.png)を流用。本番アートに差し替え可。
pdxinfo の imagePath=SystemAssets で参照される。
  python3 tools/gen_launcher.py
"""
import os
from PIL import Image

ROOT  = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TITLE = os.path.join(ROOT, "Source/images/title.png")
OUT   = os.path.join(ROOT, "Source/SystemAssets")
os.makedirs(OUT, exist_ok=True)

title = Image.open(TITLE).convert("1")

# launchImage: 400x240 そのまま流用
title.save(os.path.join(OUT, "launchImage.png"))

# card: 350x155 に収めて中央配置（白背景）
card = Image.new("1", (350, 155), 1)
src = title.convert("L")
scale = min(350 / src.width, 155 / src.height)
small = src.resize((int(src.width * scale), int(src.height * scale)), Image.NEAREST)
card.paste(small.convert("1"), ((350 - small.width) // 2, (155 - small.height) // 2))
card.save(os.path.join(OUT, "card.png"))

print("saved launchImage.png / card.png")
